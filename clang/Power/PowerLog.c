#include "PowerLog.h"

__stage(1) int powerLog(int __stage(1) X, unsigned N) __stage(1) {
  if (N == 0)
    return 1;

  return (N % 2 == 0) ? powerLog(X * X, N / 2) : X * powerLog(X, N - 1);
}

__attribute__((mix(powerLog))) void *mixPowerLog(void *Ctx, unsigned N);
