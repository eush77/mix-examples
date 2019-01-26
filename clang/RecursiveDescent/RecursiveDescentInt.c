#include "RecursiveDescent.h"

#include <stddef.h>

static const char *parseTerminal(struct Terminal *T, const char *Str) {
  return *Str == T->Ch ? Str + 1 : NULL;
}

static const char *parseAlternative(struct Alternative *A, const char *Str) {
  for (struct Symbol *S = A->Sym; S != A->Sym + A->NSym; ++S) {
    if (!(Str = parseInt(S, Str)))
      return NULL;
  }
  return Str;
}

static const char *parseNonterminal(struct Nonterminal *N, const char *Str) {
  for (struct Alternative *A = N->Alt; A != N->Alt + N->NAlt; ++A) {
    const char *End = parseAlternative(A, Str);

    if (End)
      return End;
  }

  return NULL;
}

const char *parseInt(struct Symbol *S, const char *Str) {
  switch (S->T) {
  case T_Terminal:
    return parseTerminal((struct Terminal *)S->Node, Str);

  case T_Nonterminal:
    return parseNonterminal((struct Nonterminal *)S->Node, Str);
  }
}
