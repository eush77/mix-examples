#include "RecursiveDescent.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

Terminal A = {'a'};
Terminal B = {'b'};
Terminal Plus = {'+'};

extern Nonterminal AB, S;

Symbol Symbols[] = {{T_Terminal, &A},
                    {T_Terminal, &B},
                    {T_Nonterminal, &AB},
                    {T_Terminal, &Plus},
                    {T_Nonterminal, &S}};
Symbol *Start = Symbols + 4;

Alternative Alternatives[] = {
    {1, Symbols}, {1, Symbols + 1}, {3, Symbols + 2}, {1, Symbols + 2}};
Nonterminal AB = {2, Alternatives};
Nonterminal S = {2, Alternatives + 2};

const char Text[] = "a+b+a+a+b";

void BM_RecursiveDescentInt(benchmark::State &S) {
  for (auto _: S)
    benchmark::DoNotOptimize(parseInt(Start, Text));
}

void BM_RecursiveDescentMix(benchmark::State &S) {
  Compiler C("RecursiveDescent");
  C.setFunction(mixParse(&C.getContext(), Start));
  auto *F = reinterpret_cast<const char *(*)(const char *)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(Text));
}

BENCHMARK(BM_RecursiveDescentInt);
BENCHMARK(BM_RecursiveDescentMix);

} // namespace
