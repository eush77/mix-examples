#include "Convolution.h"

static __stage(1) double sum(unsigned KH, unsigned KW,
                             struct Double Kernel[KH * KW],
                             __stage(1) unsigned H, __stage(1) unsigned W,
                             __stage(1) double Image[H * W],
                             __stage(1) unsigned Row,
                             __stage(1) unsigned Col) __stage(1) {
  double S = 0;

  for (unsigned KRow = 0; KRow < KH; ++KRow) {
    for (unsigned KCol = 0; KCol < KW; ++KCol)
      S += Kernel[KW * KRow + KCol].Val *
           Image[W * (Row - KH / 2 + KRow) + (Col - KW / 2 + KCol)];
  }

  return S;
}

void conv(unsigned KH, unsigned KW, struct Double *Kernel,
          __stage(1) unsigned H, __stage(1) unsigned W,
          __stage(1) double Image[H * W],
          __stage(1) double Out[H * W]) __stage(1) {
  for (unsigned Row = KH / 2; Row < H - KH / 2; ++Row){
    for (unsigned Col = KW / 2; Col < W - KW / 2; ++Col)
      Out[W * Row + Col] = sum(KH, KW, Kernel, H, W, Image, Row, Col);
  }
}

__attribute__((mix(conv))) void *mixConv(void *Ctx, unsigned KH, unsigned KW,
                                         struct Double *Kernel);
