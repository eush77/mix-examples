#include "Example.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdlib>

namespace {

#define PREFIX(Name, Value) const char *const Name[] = Value;
#include "Options.inc"
#undef PREFIX

namespace Option {
enum {
  INVALID,
#define OPTION(Prefixes, Name, ID, ...) ID,
#include "Options.inc"
#undef OPTION
};
} // namespace Option

const llvm::opt::OptTable::Info OptTableInfo[] = {
#define OPTION(Prefixes, Name, ID, Kind, GroupID, AliasID, AliasArgs, Flags,   \
               Param, HelpText, MetaVar, Values)                               \
  {Prefixes,        Name,       HelpText,                                      \
   MetaVar,         Option::ID, llvm::opt::Option::Kind##Class,                \
   Param,           Flags,      Option::GroupID,                               \
   Option::AliasID, AliasArgs,  Values},
#include "Options.inc"
#undef OPTION
};

class OptTable : public llvm::opt::OptTable {
public:
  OptTable() : llvm::opt::OptTable(OptTableInfo) {}
};

} // namespace

int main(int ArgC, const char *ArgV[]) {
  OptTable Options;
  unsigned MissingArgIndex, MissingArgCount;
  llvm::opt::InputArgList Args = Options.ParseArgs(
      llvm::makeArrayRef(ArgV + 1, ArgC - 1), MissingArgIndex, MissingArgCount);
  llvm::Optional<int> ExitCode;

  if (MissingArgCount) {
    llvm::errs() << "Missing value for \"" << Args.getArgString(MissingArgIndex)
                 << "\", expected " << MissingArgCount << " argument\n";
    ExitCode = EXIT_FAILURE;
  }

  for (const llvm::opt::Arg *A :
       Args.filtered(Option::UNKNOWN, Option::INPUT)) {
    llvm::errs() << "Unknown argument: \"" << A->getAsString(Args) << "\"\n";
    ExitCode = EXIT_FAILURE;
  }

  unsigned Scale = 100'000'000;
  llvm::StringRef ScaleArg = Args.getLastArgValue(Option::Scale);

  if (!ScaleArg.empty()) {
    if (ScaleArg.getAsInteger(0, Scale)) {
      llvm::errs() << "Invalid scale argument: " << ScaleArg << '\n';
      ExitCode = EXIT_FAILURE;
    }
  }

  if (!Args.hasArg(Option::Baseline, Option::Mix)) {
    llvm::errs() << "Expected --baseline, --mix, or both\n";
    ExitCode = EXIT_FAILURE;
  }

  if (Args.hasArg(Option::Help)) {
    Options.PrintHelp(llvm::outs(),
                      llvm::sys::path::filename(ArgV[0]).str().c_str(),
                      "llvm.mix example");

    if (!ExitCode)
      ExitCode = EXIT_SUCCESS;
  }

  if (ExitCode)
    return *ExitCode;

  Example Ex(Scale);

  if (Args.hasArg(Option::Baseline))
    Ex.runBaseline(2);

  if (Args.hasArg(Option::Mix))
    Ex.runMix(2);
}
