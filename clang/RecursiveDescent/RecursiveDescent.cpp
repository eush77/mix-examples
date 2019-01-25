#include "RecursiveDescent.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

Terminal TermA = {'a'};
Symbol SymA = {T_Terminal, &TermA};
Terminal TermB = {'b'};
Symbol SymB = {T_Terminal, &TermB};
Alternative A_B[] = {{1, &SymA}, {1, &SymB}};
Nonterminal AB = {2, A_B};
Symbol SymAB = {T_Nonterminal, &AB};

const char Text[] = "abab";

void BM_RecursiveDescent(benchmark::State &S) {
  for (auto _: S)
    benchmark::DoNotOptimize(parse(&SymAB, Text));
}

void BM_RecursiveDescentMix(benchmark::State &S) {
  Compiler C("RecursiveDescent");
  C.setFunction(mixParse(&C.getContext(), &SymAB));
  auto *F = reinterpret_cast<const char *(*)(const char *)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(Text));
}

BENCHMARK(BM_RecursiveDescent);
BENCHMARK(BM_RecursiveDescentMix);

} // namespace
