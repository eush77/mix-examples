#include "Bytecode.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

using namespace std::string_literals;

namespace {

Instruction Fibonacci[] = {{O_Par, 0, 0}, {O_Int, 1, 0}, {O_Int, 2, 1},
                           {O_Jze, 0, 7}, {O_Mov, 3, 1}, {O_Add, 1, 2},
                           {O_Mov, 2, 3}, {O_Int, 3, 1}, {O_Sub, 0, 3},
                           {O_Jmp, -6},   {O_Ret, 1}};
int Params[] = {8};

void BM_Bytecode(benchmark::State &S,
                 int eval(unsigned ProgramSize, Instruction *Program,
                          int *Args)) {
  for (auto _: S)
    benchmark::DoNotOptimize(
        eval(sizeof(Fibonacci) / sizeof(*Fibonacci), Fibonacci, Params));
}

void BM_BytecodeMix(benchmark::State &S, const char *Name,
                    void *mix(void *Ctx, unsigned ProgramSize,
                              Instruction *Program)) {
  Compiler C("Bytecode."s + Name);
  C.setFunction(
      mix(&C.getContext(), sizeof(Fibonacci) / sizeof(*Fibonacci), Fibonacci));
  auto *F = reinterpret_cast<int (*)(int *)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(Params));
}

BENCHMARK_CAPTURE(BM_Bytecode, Dyn, evalDyn);
BENCHMARK_CAPTURE(BM_BytecodeMix, Dyn, "Dyn", mixEvalDyn);
BENCHMARK_CAPTURE(BM_Bytecode, Base, eval);
BENCHMARK_CAPTURE(BM_BytecodeMix, Base, "Base", mixEval);
BENCHMARK_CAPTURE(BM_Bytecode, Opt, evalOpt);
BENCHMARK_CAPTURE(BM_BytecodeMix, Opt, "Opt", mixEvalOpt);

} // namespace
