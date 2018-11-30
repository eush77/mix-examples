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
#include "llvm/Support/raw_ostream.h"

#include <chrono>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

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

struct Options {
  unsigned Scale;               // Number of times to run the example
  bool PrintResult;             // Print result value
};

using BaselineTiming = std::chrono::steady_clock::duration;
using MixTiming =
    llvm::Expected<std::pair<std::chrono::steady_clock::duration,
                             std::chrono::steady_clock::duration>>;

// Overview string included in --help output
extern const std::string Overview;

// Names of arguments
extern const std::vector<std::string> ArgNames;

// Parse command-line argument
llvm::Expected<Value> parseArg(const llvm::opt::Arg *, unsigned ArgNum,
                               const llvm::opt::ArgList &);

// Run baseline example
BaselineTiming runBaseline(const Options &Opts,
                           const llvm::SmallVectorImpl<Value> &Args);

// Run mix example
MixTiming runMix(const Options &Opts, const llvm::SmallVectorImpl<Value> &Args);

template <typename FuncT>
std::chrono::steady_clock::duration runScaled(const Options &Opts, FuncT F) {
  decltype(F()) R{};
  auto Start = std::chrono::steady_clock::now();

  for (unsigned Scale = Opts.Scale; Scale--;)
    R = F();

  auto End = std::chrono::steady_clock::now();

  if (Opts.PrintResult)
    llvm::outs() << "Result: " << R << '\n';

  return End - Start;
}

template <typename FuncT, typename Result = std::result_of_t<FuncT()>>
BaselineTiming runBaseline(const Options &Opts, FuncT F) {
  return runScaled(Opts, F);
}

llvm::Error
runMix(llvm::function_ref<llvm::Function *(llvm::LLVMContext &)> RunStage0,
       llvm::function_ref<void(llvm::JITTargetAddress)> RunStage1);

template <typename RunStage0T, typename RunStage1T>
MixTiming runMix(const Options &Opts, RunStage0T RunStage0,
                 RunStage1T RunStage1) {
  std::chrono::steady_clock::time_point Start0, End0;
  Start0 = std::chrono::steady_clock::now();

  std::chrono::steady_clock::duration Time1;

  if (auto Err = runMix(RunStage0, [&](llvm::JITTargetAddress Address) {
        End0 = std::chrono::steady_clock::now();
        Time1 = runScaled(Opts, [&, Address]() { return RunStage1(Address); });
      }))
    return std::move(Err);

  return std::make_pair(End0 - Start0, Time1);
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
