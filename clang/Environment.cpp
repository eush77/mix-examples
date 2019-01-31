#include "Environment.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdlib>

bool Dumping = false;
unsigned OptLevel = 1;
unsigned SizeLevel = 0;

namespace {

template <typename T>
void parse(const char *Name, T &Val, unsigned MaxValue) {
  const char *Str = std::getenv(Name);

  if (!Str)
    return;

  unsigned Tmp;

  if (!llvm::to_integer(Str, Tmp) || Tmp > MaxValue) {
    llvm::errs() << "Invalid value for " << Name << ": \"";
    llvm::printEscapedString(Str, llvm::errs());
    llvm::errs() << "\"\n";
    return;
  }

  Val = Tmp;
}

__attribute__((constructor)) void parseDumping() {
  parse("MIX_DUMP", Dumping, 1);
}

__attribute__((constructor)) void parseOptLevel() {
  parse("MIX_OPT_LEVEL", OptLevel, 3);
}

__attribute__((constructor)) void parseSizeLevel() {
  parse("MIX_SIZE_LEVEL", SizeLevel, 2);
}

} // namespace
