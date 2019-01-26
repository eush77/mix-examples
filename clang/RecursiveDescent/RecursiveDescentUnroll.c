#include "RecursiveDescent.h"

#include <stddef.h>

static __stage(1) const
    char *parseTerminal(struct Terminal *T,
                        __stage(1) const char *Str) __stage(1) {
  return *Str == T->Ch ? Str + 1 : NULL;
}

static __stage(1) const
    char *parseThreeSymbols(struct Symbol *S0, struct Symbol *S1,
                            struct Symbol *S2,
                            __stage(1) const char *Str) __stage(1) {
  if (!(Str = S0->Parse(Str)))
    return NULL;

  if (!(Str = S1->Parse(Str)))
    return NULL;

  return S2->Parse(Str);
}

static __stage(1) const
    char *parseSymbols(unsigned NSym, struct Symbol *Sym,
                       __stage(1) const char *Str) __stage(1) {
  for (struct Symbol *S = Sym; S != Sym + NSym; ++S) {
    if (!(Str = S->Parse(Str)))
      return NULL;
  }
  return Str;
}

static __stage(1) const
    char *parseAlternative(struct Alternative *A,
                           __stage(1) const char *Str) __stage(1) {
  switch (A->NSym) {
  case 3:
    return parseThreeSymbols(A->Sym, A->Sym + 1, A->Sym + 2, Str);

  default:
    return parseSymbols(A->NSym, A->Sym, Str);
  }
}

static __stage(1) const
    char *parseTwoAlternatives(struct Alternative *A0, struct Alternative *A1,
                               __stage(1) const char *Str) __stage(1) {
  const char *End = A0->Parse(Str);

  if (End)
    return End;

  return A1->Parse(Str);
}

static __stage(1) const
    char *parseAlternatives(unsigned NAlt, struct Alternative *Alt,
                            __stage(1) const char *Str) __stage(1) {
  for (struct Alternative *A = Alt; A != Alt + NAlt; ++A) {
    const char *End = A->Parse(Str);

    if (End)
      return End;
  }

  return NULL;
}

static __stage(1) const
    char *parseNonterminal(struct Nonterminal *N,
                           __stage(1) const char *Str) __stage(1) {
  switch (N->NAlt) {
  case 2:
    return parseTwoAlternatives(N->Alt, N->Alt + 1, Str);

  default:
    return parseAlternatives(N->NAlt, N->Alt, Str);
  }
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

__attribute__((mix(parse))) void *
mixParseUnroll(void *Ctx, unsigned NumSymbols, struct Symbol Symbols[],
               unsigned NumAlternatives, struct Alternative Alternatives[],
               struct Symbol *Start);
