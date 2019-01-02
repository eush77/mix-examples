#include "Dumper.h"
#include "Environment.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ErrorHandling.h"

#include <system_error>

llvm::StringSet<> Dumper::Names;

Dumper::Dumper(llvm::StringRef Name) {
  if (!::Dumping || !Names.insert(Name).second)
    return;

  std::error_code EC;
  OS.emplace((Name + ".ll").str(), EC);

  if (EC)
    llvm::report_fatal_error(EC.message());
}

void Dumper::dump(const llvm::Module &M) {
  if (OS)
    *OS << M;
}
