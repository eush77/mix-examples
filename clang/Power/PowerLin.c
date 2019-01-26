#include "PowerLin.h"

__stage(1) int powerLin(int __stage(1) X, unsigned N) __stage(1) {
  int Res = 1;

  while (N--)
    Res *= X;

  return Res;
}

__attribute__((mix(powerLin))) void *mixPowerLin(void *Ctx, unsigned N);
