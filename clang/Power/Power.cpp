#include "../Compiler.h"
#include "PowerLin.h"
#include "PowerLog.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

void BM_PowerLin(benchmark::State &S) {
  unsigned N = S.range(0);

  for (auto _ : S)
    benchmark::DoNotOptimize(powerLin(2, N));
}

void BM_PowerLinMix(benchmark::State &S) {
  unsigned N = S.range(0);
  Compiler C("PowerLin." + std::to_string(N));
  C.setFunction(mixPowerLin(&C.getContext(), N));
  auto *F = reinterpret_cast<int (*)(int)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(2));
}

void BM_PowerLog(benchmark::State &S) {
  unsigned N = S.range(0);

  for (auto _ : S)
    benchmark::DoNotOptimize(powerLog(2, N));
}

void BM_PowerLogMix(benchmark::State &S) {
  unsigned N = S.range(0);
  Compiler C("PowerLog." + std::to_string(N));
  C.setFunction(mixPowerLog(&C.getContext(), N));
  auto *F = reinterpret_cast<int (*)(int)>(C.compile());

  for (auto _ : S)
    benchmark::DoNotOptimize(F(2));
}

BENCHMARK(BM_PowerLin)->Arg(10);
BENCHMARK(BM_PowerLinMix)->Arg(10);
BENCHMARK(BM_PowerLog)->Arg(10);
BENCHMARK(BM_PowerLogMix)->Arg(10);

} // namespace
