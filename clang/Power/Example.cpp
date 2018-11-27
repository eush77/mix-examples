#include "../Example.h"
#include "Power.h"

#include <string>

namespace llvm {
class Function;
class LLVMContext;
}

const std::string example::Overview =
    "power(x, n) - raise X to the power of N (iterative)";

#define N 5

int example::runBaseline(int X) { return power(X, N); }

llvm::Function *example::mixStage1(llvm::LLVMContext &Ctx) {
  return static_cast<llvm::Function *>(mixPower(&Ctx, N));
}
