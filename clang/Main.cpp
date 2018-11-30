#include "Example.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include <iterator>
#include <numeric>
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

// Print --help
void printHelp(llvm::StringRef ToolName, const OptTable &Options) {
  std::string Usage =
      std::accumulate(example::ArgNames.begin(), example::ArgNames.end(),
                      ToolName.str() + " [options]",
                      [](const std::string &Left, const std::string &Right) {
                        return Left + " <" + Right + ">";
                      });

  Options.PrintHelp(llvm::outs(), Usage.c_str(), example::Overview.c_str());
}

// Mode of operation
enum Mode {
  M_Baseline = 1,
  M_Mix = 2,
};

// Get mode
Mode getMode(const llvm::opt::ArgList &Args) {
  unsigned M = 0;

  for (const llvm::opt::Arg *Arg : Args.filtered(Option::Mode)) {
    switch (Arg->getOption().getID()) {
    case Option::Baseline:
      M |= M_Baseline;
      break;

    case Option::Mix:
      M |= M_Mix;
      break;

    case Option::Both:
      M |= M_Baseline | M_Mix;
      break;

    default:
      llvm_unreachable("Unhandled option");
    }
  }

  if (!M)
    M = M_Baseline | M_Mix;

  return static_cast<Mode>(M);
}

// Get --scale
llvm::Expected<unsigned> getScale(const llvm::opt::ArgList &Args) {
  unsigned Scale = 100'000'000; // Default value

  if (const llvm::opt::Arg *ScaleArg = Args.getLastArg(Option::Scale)) {
    double FScale;

    if (llvm::StringRef(ScaleArg->getValue()).getAsDouble(FScale) ||
        static_cast<double>(Scale = FScale) != FScale)
      return llvm::make_error<example::InvalidArgumentValueError>(ScaleArg,
                                                                  Args);
  }

  return Scale;
}

// Get input arguments for the example
template <typename OutputIt>
llvm::Error getInputArgValues(const llvm::opt::ArgList &Args, OutputIt Out) {
  unsigned ArgNum = 0;

  for (const llvm::opt::Arg *Arg : Args.filtered(Option::INPUT)) {
    if (llvm::Expected<example::Value> V =
            example::parseArg(Arg, ArgNum++, Args))
      *Out++ = *V;
    else
      return V.takeError();
  }

  if (ArgNum != example::ArgNames.size())
    return llvm::make_error<llvm::StringError>("Invalid number of arguments",
                                               llvm::inconvertibleErrorCode());

  return llvm::Error::success();
}

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
    printHelp(ToolName, Options);
    return {};
  }

  Mode M = getMode(Args);
  unsigned Scale = ExitOnError(getScale(Args));

  llvm::SmallVector<example::Value, 4> InputArgValues;
  ExitOnError(getInputArgValues(Args, std::back_inserter(InputArgValues)));

  if (M & M_Baseline)
    example::runBaseline(Scale, InputArgValues);

  if (M & M_Mix)
    ExitOnError(example::runMix(Scale, InputArgValues));
}
