#include "RecursiveDescent.h"

#include <stddef.h>

static __stage(1) const
    char *parseTerminal(struct Terminal *T,
                        __stage(1) const char *Str) __stage(1) {
  return *Str == T->Ch ? Str + 1 : NULL;
}

static __stage(1) const
    char *parseAlternative(struct Alternative *A,
                           __stage(1) const char *Str) __stage(1) {
  for (struct Symbol *S = A->Sym; S != A->Sym + A->NSym; ++S) {
    if (!(Str = S->Parse(Str)))
      return NULL;
  }
  return Str;
}

static __stage(1) const
    char *parseNonterminal(struct Nonterminal *N,
                           __stage(1) const char *Str) __stage(1) {
  for (struct Alternative *A = N->Alt; A != N->Alt + N->NAlt; ++A) {
    const char *End = A->Parse(Str);

    if (End)
      return End;
  }

  return NULL;
}

static __stage(1) Thunk compileNonterminal(struct Nonterminal *N) __stage(1) {
  return N->NAlt == 1 ? N->Alt->Parse : __builtin_mix_call(parseNonterminal, N);
}

static __stage(1) Thunk compileSymbol(struct Symbol *S) __stage(1) {
  switch (S->T) {
  case T_Terminal:
    return __builtin_mix_call(parseTerminal, (struct Terminal *)S->Node);

  case T_Nonterminal:
    return compileNonterminal(S->Node);
  }
}

static __stage(1) const
    char *parse(unsigned NumSymbols, struct Symbol Symbols[],
                unsigned NumAlternatives, struct Alternative Alternatives[],
                struct Symbol *Start, __stage(1) const char *Str) __stage(1) {
  for (struct Alternative *A = Alternatives;
       A != Alternatives + NumAlternatives; ++A)
    // If alternative contains one symbol, resolve it later
    A->Parse = A->NSym == 1 ? NULL : __builtin_mix_call(parseAlternative, A);

  for (struct Symbol *S = Symbols; S != Symbols + NumSymbols; ++S)
    S->Parse = compileSymbol(S);

  // Resolve one-symbol alternatives
  for (struct Alternative *A = Alternatives;
       A != Alternatives + NumAlternatives; ++A)
    if (A->NSym == 1)
      A->Parse = A->Sym->Parse;

  return Start->Parse(Str);
}

__attribute__((mix(parse))) void *mixParse(void *Ctx, unsigned NumSymbols,
                                           struct Symbol Symbols[],
                                           unsigned NumAlternatives,
                                           struct Alternative Alternatives[],
                                           struct Symbol *Start);
