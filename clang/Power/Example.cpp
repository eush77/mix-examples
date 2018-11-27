#include "../Example.h"
#include "Power.h"

namespace llvm {
class Function;
class LLVMContext;
}

#define N 5

int example::runBaseline(int X) { return power(X, N); }

llvm::Function *example::mixStage1(llvm::LLVMContext &Ctx) {
  return static_cast<llvm::Function *>(mixPower(&Ctx, N));
}
