#include "../Compiler.h"
#include "PowerIter.h"
#include "PowerRec.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

void BM_PowerIter(benchmark::State &S) {
  unsigned N = S.range(0);

  for (auto _ : S)
    benchmark::DoNotOptimize(powerIter(2, N));
}

void BM_PowerIterMix(benchmark::State &S) {
  unsigned N = S.range(0);
  Compiler C("PowerIter." + std::to_string(N));
  C.setFunction(mixPowerIter(&C.getContext(), N));
  auto *F = reinterpret_cast<int (*)(int)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(2));
}

void BM_PowerRec(benchmark::State &S) {
  unsigned N = S.range(0);

  for (auto _ : S)
    benchmark::DoNotOptimize(powerRec(2, N));
}

void BM_PowerRecMix(benchmark::State &S) {
  unsigned N = S.range(0);
  Compiler C("PowerRec." + std::to_string(N));
  C.setFunction(mixPowerRec(&C.getContext(), N));
  auto *F = reinterpret_cast<int (*)(int)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(2));
}

BENCHMARK(BM_PowerIter)->RangeMultiplier(2)->Range(5, 40)->Complexity();
BENCHMARK(BM_PowerIterMix)->RangeMultiplier(2)->Range(5, 40)->Complexity();
BENCHMARK(BM_PowerRec)->RangeMultiplier(2)->Range(5, 40)->Complexity();
BENCHMARK(BM_PowerRecMix)->RangeMultiplier(2)->Range(5, 40)->Complexity();

} // namespace
