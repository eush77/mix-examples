#ifndef MIX_EXAMPLES_CLANG_CONVOLUTION_CONVOLUTION_H
#define MIX_EXAMPLES_CLANG_CONVOLUTION_CONVOLUTION_H

#ifdef __cplusplus
extern "C" {
#endif

struct Double {
  double Val;
} __attribute__((packed,staged));

void conv(unsigned KH, unsigned KW, struct Double Kernel[], unsigned H,
          unsigned W, double Image[], double Out[]);
void *mixConv(void *Ctx, unsigned KH, unsigned KW, struct Double Kernel[]);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MIX_EXAMPLES_CLANG_CONVOLUTION_CONVOLUTION_H
