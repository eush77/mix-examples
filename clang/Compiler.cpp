#include "Compiler.h"
#include "Environment.h"

#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

using namespace std::placeholders;

namespace {

class Target {
public:
  static llvm::TargetMachine &getTargetMachine() { return *getInstance().TM; }
  static const llvm::DataLayout &getDataLayout() { return *getInstance().DL; }

private:
  Target() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    TM.reset(llvm::EngineBuilder().selectTarget());
    DL.emplace(TM->createDataLayout());
  }

  static const Target &getInstance() {
    static Target T;
    return T;
  }

  std::unique_ptr<llvm::TargetMachine> TM;
  llvm::Optional<llvm::DataLayout> DL;
};

} // namespace

Compiler::Compiler(llvm::StringRef Name)
    : D(Name),
      ES(std::make_unique<llvm::orc::UnsupportedExecutorProcessControl>()),
      JD(cantFail(ES.createJITDylib("<main>"))),
      ObjectLayer(ES, std::bind(&std::make_unique<llvm::SectionMemoryManager>)),
      CompileLayer(ES, ObjectLayer,
                   std::make_unique<llvm::orc::SimpleCompiler>(
                       Target::getTargetMachine())),
      Ctx(new llvm::LLVMContext) {
  JD.addGenerator(cantFail(
      llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
          Target::getDataLayout().getGlobalPrefix())));
}

namespace {

void optimizeModule(llvm::Module &M) {
  llvm::PassManagerBuilder PMB;
  PMB.OptLevel = ::OptLevel;
  PMB.SizeLevel = ::SizeLevel;
  PMB.Inliner =
      llvm::createFunctionInliningPass(::OptLevel, ::SizeLevel, false);

  {
    llvm::legacy::FunctionPassManager FPM(&M);
    PMB.populateFunctionPassManager(FPM);

    FPM.doInitialization();
    std::for_each(M.begin(), M.end(),
                  std::bind(&llvm::legacy::FunctionPassManager::run, &FPM, _1));
    FPM.doFinalization();
  }

  {
    llvm::legacy::PassManager MPM;
    PMB.populateModulePassManager(MPM);
    MPM.run(M);
  }
}

} // namespace

void Compiler::setFunction(void *VoidF) {
  F = static_cast<llvm::Function *>(VoidF);
  M = F->getParent();

  M->setDataLayout(Target::getDataLayout());
  optimizeModule(*M);
  D.dump(*M);
}

llvm::JITTargetAddress Compiler::compile() {
  llvm::orc::ThreadSafeModule TSM(std::unique_ptr<llvm::Module>(M),
                                  std::move(Ctx));

  if (llvm::Error Err = CompileLayer.add(JD, std::move(TSM)))
    llvm::report_fatal_error(std::move(Err));

  llvm::Expected<llvm::JITEvaluatedSymbol> Symbol = JD.withLinkOrderDo(
      [&](const llvm::orc::JITDylibSearchOrder &SearchOrder) {
        return ES.lookup(SearchOrder, ES.intern(F->getName()));
      });

  if (!Symbol)
    llvm::report_fatal_error(Symbol.takeError());

  return Symbol->getAddress();
}
