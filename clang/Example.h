// The interface implemented by all examples

#ifndef MIX_EXAMPLES_CLANG_EXAMPLE_H
#define MIX_EXAMPLES_CLANG_EXAMPLE_H

namespace llvm {
class Function;
class LLVMContext;
}

class Example {
public:
  explicit Example(unsigned Scale) : Scale(Scale) {}

  int runBaseline(int X) const { return runScaled(&runBaselineOnce, X); }
  int runMix(int X) const;

private:
  // Run the baseline function
  static int runBaselineOnce(int X);

  // Build stage(1) function in a given LLVMContext
  static llvm::Function *mixStage1(llvm::LLVMContext &Ctx);

  template<typename Func>
  int runScaled(Func F, int X) const;

  unsigned Scale;
};

template <typename Func> int Example::runScaled(Func F, int X) const {
  int Res = 0;

  for (unsigned S = Scale; S--;)
    Res = F(X);

  return Res;
}

#endif // MIX_EXAMPLES_CLANG_EXAMPLE_H
