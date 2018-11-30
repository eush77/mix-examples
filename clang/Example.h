// The interface implemented by all examples

#ifndef MIX_EXAMPLES_CLANG_EXAMPLE_H
#define MIX_EXAMPLES_CLANG_EXAMPLE_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Error.h"

#include <string>
#include <system_error>
#include <type_traits>

namespace llvm {
class Function;
class LLVMContext;
}

namespace example {

union Value {
  int AsInt;
  unsigned AsUnsigned;

  Value(int V) : AsInt(V) {}
  Value(unsigned V) : AsUnsigned(V) {}
};

// Overview string included in --help output
extern const std::string Overview;

// Number of arguments
extern const unsigned NumArgs;

// Parse command-line argument
llvm::Expected<Value> parseArg(const llvm::opt::Arg *, unsigned ArgNum,
                               const llvm::opt::ArgList &);

// Run baseline example
void runBaseline(unsigned Scale, const llvm::SmallVectorImpl<Value> &Args);

// Run mix example
llvm::Error runMix(unsigned Scale, const llvm::SmallVectorImpl<Value> &Args);

template <typename FuncT>
auto runScaled(unsigned Scale, FuncT F) -> decltype(F()) {
  decltype(F()) R{};

  while (Scale--)
    R = F();

  return R;
}

template <typename FuncT, typename Result = std::result_of_t<FuncT()>>
void runBaseline(unsigned Scale, FuncT F) {
  runScaled(Scale, F);
}

llvm::Error
runMix(llvm::function_ref<llvm::Function *(llvm::LLVMContext &)> RunStage0,
       llvm::function_ref<void(llvm::JITTargetAddress)> RunStage1);

template <typename RunStage0T, typename RunStage1T>
llvm::Error runMix(unsigned Scale, RunStage0T RunStage0, RunStage1T RunStage1) {
  return runMix(RunStage0, [&, Scale](llvm::JITTargetAddress Address) {
    runScaled(Scale, [&, Address]() { return RunStage1(Address); });
  });
}

class InvalidArgumentError : public llvm::ErrorInfo<InvalidArgumentError> {
public:
  static char ID;

  InvalidArgumentError(const llvm::opt::Arg *Arg,
                       const llvm::opt::ArgList &ArgList)
      : Arg(Arg), ArgList(ArgList) {}

  void log(llvm::raw_ostream &OS) const override {
    OS << "Invalid argument: " << Arg->getAsString(ArgList);
  }

  std::error_code convertToErrorCode() const override {
    return llvm::inconvertibleErrorCode();
  }

private:
  const llvm::opt::Arg *Arg;
  const llvm::opt::ArgList &ArgList;
};

class InvalidArgumentValueError
    : public llvm::ErrorInfo<InvalidArgumentValueError> {
public:
  static char ID;

  InvalidArgumentValueError(const llvm::opt::Arg *Arg,
                            const llvm::opt::ArgList &ArgList)
      : Arg(Arg), ArgList(ArgList) {}

  void log(llvm::raw_ostream &OS) const override {
    OS << "Invalid value for " << Arg->getAsString(ArgList) << ": "
       << Arg->getValue();
  }

  std::error_code convertToErrorCode() const override {
    return llvm::inconvertibleErrorCode();
  }

private:
  const llvm::opt::Arg *Arg;
  const llvm::opt::ArgList &ArgList;
};

template <typename ValueT>
llvm::Expected<Value> parseArgAs(const llvm::opt::Arg *Arg,
                                 const llvm::opt::ArgList &ArgList) {
  ValueT V;

  if (llvm::StringRef(Arg->getValue()).getAsInteger<ValueT>(0, V))
    return llvm::make_error<InvalidArgumentError>(Arg, ArgList);

  return V;
}

} // example

#endif // MIX_EXAMPLES_CLANG_EXAMPLE_H
