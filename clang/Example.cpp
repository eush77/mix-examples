#include "Example.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

#include <functional>
#include <memory>
#include <utility>

char example::IncompatibleArgumentsError::ID;
char example::InvalidArgumentError::ID;
char example::InvalidArgumentValueError::ID;

llvm::Error example::runMix(
    llvm::function_ref<llvm::Function *(llvm::LLVMContext &)> RunStage0,
    llvm::function_ref<void(llvm::JITTargetAddress)> RunStage1) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  std::unique_ptr<llvm::TargetMachine> TM(llvm::EngineBuilder().selectTarget());
  const llvm::DataLayout DL(TM->createDataLayout());

  // Build the ORC stack
  llvm::orc::ExecutionSession ES;
  llvm::orc::RTDyldObjectLinkingLayer ObjectLayer(
      ES, std::bind(&std::make_unique<llvm::SectionMemoryManager>));
  llvm::orc::IRCompileLayer CompileLayer(ES, ObjectLayer,
                                         llvm::orc::SimpleCompiler(*TM));

  // Create the stage(1) IR
  auto Ctx = std::make_unique<llvm::LLVMContext>();
  llvm::Function *Stage1 = RunStage0(*Ctx);
  Stage1->getParent()->setDataLayout(DL);
  llvm::orc::ThreadSafeModule TSM(
      std::unique_ptr<llvm::Module>(Stage1->getParent()), std::move(Ctx));

  if (llvm::Error Err = CompileLayer.add(ES.getMainJITDylib(), std::move(TSM)))
    return std::move(Err);

  llvm::Expected<llvm::JITEvaluatedSymbol> Symbol =
      ES.getMainJITDylib().withSearchOrderDo(
          [&](const llvm::orc::JITDylibSearchList &SearchList) {
            return ES.lookup(SearchList, ES.intern(Stage1->getName()));
          });

  if (!Symbol)
    return Symbol.takeError();

  RunStage1(Symbol->getAddress());
  return llvm::Error::success();
}
