#include "Convolution.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

double Image[100][100];
double Out[100][100];
double Kernel[3][3] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};

void BM_Convolution(benchmark::State &S) {
  for (auto _: S)
    conv(3, 3, (Double *)Kernel, 100, 100, (double *)Image, (double *)Out);
}

void BM_ConvolutionMix(benchmark::State &S) {
  Compiler C("Convolution");
  C.setFunction(mixConv(&C.getContext(), 3, 3, (Double *)Kernel));
  auto *F = reinterpret_cast<void (*)(unsigned, unsigned, double *, double *)>(
      C.compile());

  for (auto _ : S)
    F(100, 100, (double *)Image, (double *)Out);
}

BENCHMARK(BM_Convolution);
BENCHMARK(BM_ConvolutionMix);

} // namespace
