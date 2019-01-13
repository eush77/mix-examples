#include "Bytecode.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

void BM_Bytecode(benchmark::State &S, Instruction *Program, int *Args) {
  for (auto _: S)
    benchmark::DoNotOptimize(eval(Program, Args));
}

void BM_BytecodeMix(benchmark::State &S, const std::string &Name,
                    Instruction *Program, int *Args) {
  Compiler C("Bytecode." + Name);
  C.setFunction(mixEval(&C.getContext(), Program));
  auto *F = reinterpret_cast<int (*)(int *)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(Args));
}

static Instruction Fibonacci[] = {{O_Par, 0, 0},  {O_Int, 1, 0}, {O_Int, 2, 1},
                                  {O_Jze, 0, 10}, {O_Mov, 3, 1}, {O_Add, 1, 2},
                                  {O_Mov, 2, 3},  {O_Int, 3, 1}, {O_Sub, 0, 3},
                                  {O_Jmp, 3},     {O_Ret, 1}};
static int Params[] = {8};

BENCHMARK_CAPTURE(BM_Bytecode, Fibonacci, Fibonacci, Params);
BENCHMARK_CAPTURE(BM_BytecodeMix, Fibonacci, "Fibonacci", Fibonacci, Params);

} // namespace
