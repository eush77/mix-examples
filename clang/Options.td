include "llvm/Option/OptParser.td"

def Help : Flag<["--"], "help">;

def Mode : OptionGroup<"Mode">, HelpText<"MODE">;
def Baseline : Flag<["--"], "baseline">,
               HelpText<"Run the baseline">,
               Group<Mode>;
def Mix : Flag<["--"], "mix">, HelpText<"Run the mix">, Group<Mode>;
def Both : Flag<["--"], "both">, HelpText<"Run both (default)">, Group<Mode>;

def Dump : Flag<["--"], "dump">, HelpText<"Dump stage(1) IR to stdout">;
def DumpToFile : Joined<["--"], "dump=">,
                 HelpText<"Dump stage(1) IR to file">,
                 MetaVarName<"<file>">;
def Result : Flag<["--"], "result">, HelpText<"Print results">;
def Scale : Joined<["--"], "scale=">, HelpText<"Number of iterations">;
def Timing : Flag<["--"], "timing">, HelpText<"Print execution time">;
