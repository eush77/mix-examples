#include "PowerIter.h"

__stage(1) int powerIter(int __stage(1) X, unsigned N) __stage(1) {
  int Res = 1;

  while (N--)
    Res *= X;

  return Res;
}

__attribute__((mix_ir(powerIter))) void *mixPowerIter(void *Ctx, unsigned N);
