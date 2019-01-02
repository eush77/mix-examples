#ifndef MIX_EXAMPLES_CLANG_COMPILER_H
#define MIX_EXAMPLES_CLANG_COMPILER_H

#include "Dumper.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/IR/LLVMContext.h"

#include <memory>

namespace llvm {

class Function;
class Module;

} // namespace llvm

class Compiler {
public:
  explicit Compiler(llvm::StringRef Name);

  llvm::LLVMContext &getContext() { return *Ctx; }
  void setFunction(void *);
  llvm::JITTargetAddress compile();

private:
  Dumper D;
  llvm::orc::ExecutionSession ES;
  llvm::orc::RTDyldObjectLinkingLayer ObjectLayer;
  llvm::orc::IRCompileLayer CompileLayer;
  std::unique_ptr<llvm::LLVMContext> Ctx;
  llvm::Function *F = nullptr;
  llvm::Module *M = nullptr;
};

#endif // MIX_EXAMPLES_CLANG_COMPILER_H
