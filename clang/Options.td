include "llvm/Option/OptParser.td"

def Baseline : Flag<["--"], "baseline">, HelpText<"Run the baseline">;
def Help : Flag<["--"], "help">, HelpText<"Print available options">;
def Mix : Flag<["--"], "mix">, HelpText<"Run the mix">;
def Scale : Joined<["--"], "scale=">, HelpText<"Number of iterations">;
