// The interface implemented by all examples

#ifndef MIX_EXAMPLES_CLANG_EXAMPLE_H
#define MIX_EXAMPLES_CLANG_EXAMPLE_H

#include <string>

namespace llvm {
class Function;
class LLVMContext;
}

namespace example {

extern const std::string Overview;

// Baseline
int runBaseline(int X);
int runBaseline(unsigned Scale, int X);

// Mix
llvm::Function *mixStage1(llvm::LLVMContext &Ctx);
int runMix(unsigned Scale, int X);

} // namespace example

#endif // MIX_EXAMPLES_CLANG_EXAMPLE_H
