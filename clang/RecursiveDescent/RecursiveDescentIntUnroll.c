#include "RecursiveDescent.h"

#include <stddef.h>

static const char *parseTerminal(struct Terminal *T, const char *Str) {
  return *Str == T->Ch ? Str + 1 : NULL;
}

static const char *parseThreeSymbols(struct Symbol *S0, struct Symbol *S1,
                                     struct Symbol *S2, const char *Str) {
  if (!(Str = parseIntUnroll(S0, Str)))
    return NULL;

  if (!(Str = parseIntUnroll(S1, Str)))
    return NULL;

  return parseIntUnroll(S2, Str);
}

static const char *parseSymbols(unsigned NSym, struct Symbol *Sym,
                                const char *Str) {
  for (struct Symbol *S = Sym; S != Sym + NSym; ++S) {
    if (!(Str = parseIntUnroll(S, Str)))
      return NULL;
  }
  return Str;
}

static const char *parseAlternative(struct Alternative *A, const char *Str) {
  switch (A->NSym) {
  case 1:
    return parseIntUnroll(A->Sym, Str);

  case 3:
    return parseThreeSymbols(A->Sym, A->Sym + 1, A->Sym + 2, Str);

  default:
    return parseSymbols(A->NSym, A->Sym, Str);
  }
}

static const char *parseTwoAlternatives(struct Alternative *A0,
                                        struct Alternative *A1,
                                        const char *Str) {
  const char *End = parseAlternative(A0, Str);

  if (End)
    return End;

  return parseAlternative(A1, Str);
}

static const char *parseAlternatives(unsigned NAlt, struct Alternative *Alt,
                                     const char *Str) {
  for (struct Alternative *A = Alt; A != Alt + NAlt; ++A) {
    const char *End = parseAlternative(A, Str);

    if (End)
      return End;
  }

  return NULL;
}

static const char *parseNonterminal(struct Nonterminal *N, const char *Str) {
  switch (N->NAlt) {
  case 1:
    return parseAlternative(N->Alt, Str);

  case 2:
    return parseTwoAlternatives(N->Alt, N->Alt + 1, Str);

  default:
    return parseAlternatives(N->NAlt, N->Alt, Str);
  }
}

const char *parseIntUnroll(struct Symbol *S, const char *Str) {
  switch (S->T) {
  case T_Terminal:
    return parseTerminal((struct Terminal *)S->Node, Str);

  case T_Nonterminal:
    return parseNonterminal((struct Nonterminal *)S->Node, Str);
  }
}
