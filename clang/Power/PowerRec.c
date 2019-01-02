#include "PowerRec.h"

__stage(1) int powerRec(int __stage(1) X, unsigned N) __stage(1) {
  if (N == 0)
    return 1;

  return (N % 2 == 0) ? powerRec(X * X, N / 2) : X * powerRec(X, N - 1);
}

__attribute__((mix_ir(powerRec))) void *mixPowerRec(void *Ctx, unsigned N);
