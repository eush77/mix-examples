#include "Example.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/NullResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"

#include <memory>

int Example::runMix(int X) const {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  std::unique_ptr<llvm::TargetMachine> TM(llvm::EngineBuilder().selectTarget());
  const llvm::DataLayout DL(TM->createDataLayout());

  // Build the ORC stack
  llvm::orc::RTDyldObjectLinkingLayer ObjectLayer(
      []() { return std::make_shared<llvm::SectionMemoryManager>(); });
  llvm::orc::IRCompileLayer<decltype(ObjectLayer), llvm::orc::SimpleCompiler>
      CompileLayer(ObjectLayer, llvm::orc::SimpleCompiler(*TM));

  // Create the stage(1) IR
  llvm::LLVMContext Ctx;
  llvm::Function *Stage1 = mixStage1(Ctx);
  Stage1->getParent()->setDataLayout(DL);

  llvm::SmallString<16> MangledSymbol;
  llvm::Mangler::getNameWithPrefix(MangledSymbol, Stage1->getName(), DL);

  llvm::cantFail(
      CompileLayer.addModule(std::unique_ptr<llvm::Module>(Stage1->getParent()),
                             std::make_unique<llvm::orc::NullResolver>()));

  auto Address = llvm::cantFail(
      CompileLayer.findSymbol(MangledSymbol.c_str(), false).getAddress());
  int (*Func)(int) = reinterpret_cast<int (*)(int)>(Address);

  return runScaled(Func, X);
}
