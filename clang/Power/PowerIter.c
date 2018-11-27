#include "Power.h"

__stage(1) int power(int __stage(1) X, unsigned N) __stage(1) {
  int Res = 1;

  while (N--)
    Res *= X;

  return Res;
}

__attribute__((mix_ir(power))) void *mixPower(void *Ctx, unsigned N);
