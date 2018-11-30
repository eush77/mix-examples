#include "Power.h"

__stage(1) int power(int __stage(1) X, unsigned N) __stage(1) {
  if (N == 0)
    return 1;

  return (N % 2 == 0) ? power(X * X, N / 2) : X * power(X, N - 1);
}

__attribute__((mix_ir(power))) void *mixPower(void *Ctx, unsigned N);
