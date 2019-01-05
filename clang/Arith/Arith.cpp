#include "Arith.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

void BM_Arith(benchmark::State &S, Node *Expr, int *Args) {
  for (auto _: S)
    benchmark::DoNotOptimize(eval(Expr, Args));
}

void BM_ArithMix(benchmark::State &S, const std::string &Name, Node *Expr,
                 int *Args) {
  Compiler C("Arith." + Name);
  C.setFunction(mixEval(&C.getContext(), Expr));
  auto *F = reinterpret_cast<int (*)(int *)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(Args));
}

static int Params[] = {5, 7};
static ParamNode Param0{O_Param, 0};
static ParamNode Param1{O_Param, 1};
static IntNode One{O_Int, 1};
static IntNode Two{O_Int, 2};
static BinaryNode Inc{O_Add, reinterpret_cast<Node *>(&Param0),
                      reinterpret_cast<Node *>(&One)};
static BinaryNode Mul{O_Mul, reinterpret_cast<Node *>(&Param0),
                       reinterpret_cast<Node *>(&Param1)};
static BinaryNode Add{O_Add, reinterpret_cast<Node *>(&Inc),
                      reinterpret_cast<Node *>(&Mul)};
static BinaryNode Div{O_Div, reinterpret_cast<Node *>(&Add),
                      reinterpret_cast<Node *>(&Two)};

BENCHMARK_CAPTURE(BM_Arith, Inc, reinterpret_cast<Node *>(&Inc), Params);
BENCHMARK_CAPTURE(BM_ArithMix, Inc, "Inc", reinterpret_cast<Node *>(&Inc),
                  Params);

BENCHMARK_CAPTURE(BM_Arith, Expr, reinterpret_cast<Node *>(&Div), Params);
BENCHMARK_CAPTURE(BM_ArithMix, Expr, "Expr", reinterpret_cast<Node *>(&Div),
                  Params);

} // namespace
