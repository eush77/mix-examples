#ifndef MIX_EXAMPLES_CLANG_DUMPER_H
#define MIX_EXAMPLES_CLANG_DUMPER_H

#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

class Dumper {
public:
  explicit Dumper(llvm::StringRef Name);

  void dump(const llvm::Module &M);

private:
  static llvm::StringSet<> Names;

  llvm::Optional<llvm::raw_fd_ostream> OS;
};

#endif // MIX_EXAMPLES_CLANG_DUMPER_H
