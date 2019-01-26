#include "Arith.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

int Params[] = {5, 7};
ParamNode Param0{O_Param, 0};
ParamNode Param1{O_Param, 1};
IntNode One{O_Int, 1};
IntNode Two{O_Int, 2};
BinaryNode Inc{O_Add, reinterpret_cast<Node *>(&Param0),
               reinterpret_cast<Node *>(&One)};
BinaryNode Mul{O_Mul, reinterpret_cast<Node *>(&Param0),
               reinterpret_cast<Node *>(&Param1)};
BinaryNode Add{O_Add, reinterpret_cast<Node *>(&Inc),
               reinterpret_cast<Node *>(&Mul)};
BinaryNode Div{O_Div, reinterpret_cast<Node *>(&Add),
               reinterpret_cast<Node *>(&Two)};

void BM_Arith(benchmark::State &S) {
  for (auto _: S)
    benchmark::DoNotOptimize(evalExpr(reinterpret_cast<Node *>(&Div), Params));
}

void BM_ArithMix(benchmark::State &S) {
  Compiler C("Arith");
  C.setFunction(mixEvalExpr(&C.getContext(), reinterpret_cast<Node *>(&Div)));
  auto *F = reinterpret_cast<int (*)(int *)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(Params));
}

BENCHMARK(BM_Arith);
BENCHMARK(BM_ArithMix);

} // namespace
