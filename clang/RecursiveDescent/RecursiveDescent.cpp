#include "RecursiveDescent.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

using namespace std::string_literals;

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

void BM_RecursiveDescentInt(benchmark::State &S,
                            const char *parse(Symbol *Start, const char *Str)) {
  for (auto _: S)
    benchmark::DoNotOptimize(parse(Start, Text));
}

void BM_RecursiveDescentMix(benchmark::State &S, const char *Name,
                            void *mix(void *Ctx, unsigned NumSymbols,
                                      Symbol Symbols[],
                                      unsigned NumAlternatives,
                                      Alternative Alternatives[],
                                      Symbol *Start)) {
  Compiler C("RecursiveDescent."s + Name);
  C.setFunction(mix(&C.getContext(), sizeof(Symbols) / sizeof(*Symbols),
                    Symbols, sizeof(Alternatives) / sizeof(*Alternatives),
                    Alternatives, Start));
  auto *F = reinterpret_cast<const char *(*)(const char *)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(Text));
}

BENCHMARK_CAPTURE(BM_RecursiveDescentInt, Base, parseInt);
BENCHMARK_CAPTURE(BM_RecursiveDescentMix, Base, "Base", mixParse);
BENCHMARK_CAPTURE(BM_RecursiveDescentInt, Unroll, parseIntUnroll);
BENCHMARK_CAPTURE(BM_RecursiveDescentMix, Unroll, "Unroll", mixParseUnroll);

} // namespace
