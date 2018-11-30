#include "Example.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include <iterator>
#include <string>

using namespace std::string_literals;

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
  llvm::StringRef ToolName = llvm::sys::path::filename(ArgV[0]);
  llvm::ExitOnError ExitOnError(ToolName.str() + ": ");

  OptTable Options;
  unsigned MissingArgIndex, MissingArgCount;
  llvm::opt::InputArgList Args = Options.ParseArgs(
      llvm::makeArrayRef(ArgV + 1, ArgC - 1), MissingArgIndex, MissingArgCount);

  if (MissingArgCount)
    ExitOnError(llvm::make_error<llvm::StringError>(
        "Missing value for \""s + Args.getArgString(MissingArgIndex) + "\"",
        llvm::inconvertibleErrorCode()));

  for (const llvm::opt::Arg *A : Args.filtered(Option::UNKNOWN))
    ExitOnError(llvm::make_error<example::InvalidArgumentError>(A, Args));

  if (Args.hasArg(Option::Help)) {
    Options.PrintHelp(llvm::outs(), ToolName.str().c_str(),
                      example::Overview.c_str());
    return {};
  }

  unsigned Scale = 100'000'000;

  if (const llvm::opt::Arg *ScaleArg = Args.getLastArg(Option::Scale)) {
    double FScale;

    if (llvm::StringRef(ScaleArg->getValue()).getAsDouble(FScale) ||
        static_cast<double>(Scale = FScale) != FScale)
      ExitOnError(
          llvm::make_error<example::InvalidArgumentValueError>(ScaleArg, Args));
  }

  if (!Args.hasArg(Option::Baseline, Option::Mix))
    ExitOnError(llvm::make_error<llvm::StringError>(
        "Expected --baseline, --mix, or both", llvm::inconvertibleErrorCode()));

  llvm::SmallVector<const llvm::opt::Arg *, 4> InputArgs;
  llvm::copy(Args.filtered(Option::INPUT), std::back_inserter(InputArgs));

  if (InputArgs.size() != example::NumArgs)
    ExitOnError(llvm::make_error<llvm::StringError>(
        "Invalid number of arguments", llvm::inconvertibleErrorCode()));

  llvm::SmallVector<example::Value, 4> InputArgValues;
  llvm::transform(InputArgs, std::back_inserter(InputArgValues),
                  [&](const llvm::opt::Arg *Arg) {
                    return ExitOnError(
                        example::parseArg(Arg, InputArgValues.size(), Args));
                  });

  if (Args.hasArg(Option::Baseline))
    example::runBaseline(Scale, InputArgValues);

  if (Args.hasArg(Option::Mix))
    ExitOnError(example::runMix(Scale, InputArgValues));
}
