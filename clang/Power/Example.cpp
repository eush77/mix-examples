#include "../Example.h"
#include "Power.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Error.h"

#include <string>
#include <vector>

namespace llvm {
class Function;
class LLVMContext;
}

const std::string example::Overview = "power(x, n) - raise X to the power of N";

const std::vector<std::string> example::ArgNames{"x", "n"};

llvm::Expected<example::Value>
example::parseArg(const llvm::opt::Arg *Arg, unsigned ArgNum,
                  const llvm::opt::ArgList &ArgList) {
  switch (ArgNum) {
  case 0:
    return parseArgAs<int>(Arg, ArgList);

  case 1:
    return parseArgAs<unsigned>(Arg, ArgList);

  default:
    return llvm::make_error<InvalidArgumentError>(Arg, ArgList);
  }
}

example::BaselineTiming
example::runBaseline(const example::Options &Opts,
                     const llvm::SmallVectorImpl<Value> &Args) {
  return runBaseline(Opts, [X = Args[0].AsInt, N = Args[1].AsUnsigned]() {
    return power(X, N);
  });
}

example::MixTiming example::runMix(const example::Options &Opts,
                                   const llvm::SmallVectorImpl<Value> &Args) {
  return runMix(
      Opts,
      [N = Args[1].AsUnsigned](llvm::LLVMContext &Ctx) {
        return static_cast<llvm::Function *>(mixPower(&Ctx, N));
      },
      [X = Args[0].AsInt](llvm::JITTargetAddress Address) {
        return reinterpret_cast<int (*)(int)>(Address)(X);
      });
}
