#include "Format.h"
#include "../Compiler.h"

#include "benchmark/benchmark.h"

#include <string>

namespace {

char OutBuffer[256];
const char Format[] =
    "Good morning, {0} {1}, and welcome to the Black Mesa Transit System";
const char Arg0[] = "Mr.";
const char Arg1[] = "Freeman";
const char *Args[] = {Arg0, Arg1};

void BM_Format(benchmark::State &S) {
  for (auto _: S)
    format(OutBuffer, reinterpret_cast<const Char *>(Format), Args);
}

void BM_FormatMix(benchmark::State &S) {
  Compiler C("Format");
  C.setFunction(
      mixFormat(&C.getContext(), reinterpret_cast<const Char *>(Format)));
  auto *F = reinterpret_cast<void (*)(char *, const char **)>(C.compile());

  for (auto _ : S)
    F(OutBuffer, Args);
}

BENCHMARK(BM_Format);
BENCHMARK(BM_FormatMix);

} // namespace
