#include "RecursiveDescent.h"

#include <stdbool.h>

static __stage(1) bool parseForMix(struct Symbol *,
                                   __stage(1) const char *) __stage(1);

static __stage(1) bool parseTerminal(struct Terminal *T,
                                     __stage(1) const char *Str) __stage(1) {
  return *Str == T->Ch;
}

static __stage(1) bool parseAlternative(struct Alternative *A,
                                        __stage(1) const char *Str) __stage(1) {
  bool M = true;

  for (struct Symbol *S = A->Sym; S != A->Sym + A->NSym; ++S)
    M &= parse(S, Str);

  return M;
}

static __stage(1) bool parseAlternativeForMix(struct Alternative *A,
                                              __stage(1)
                                                  const char *Str) __stage(1) {
  bool M = true;

  for (struct Symbol *S = A->Sym; S != A->Sym + A->NSym; ++S)
    M &= parseForMix(S, Str);

  return M;
}

static bool parseNonterminal(struct Nonterminal *N, const char *Str) {
  for (struct Alternative *A = N->Alt; A != N->Alt + N->NAlt; ++A)
    if (parseAlternative(A, Str))
      return true;

  return false;
}

static __stage(1) bool parseNonterminalForMix(struct Nonterminal *N,
                                              __stage(1)
                                                  const char *Str) __stage(1) {
  __builtin_assume(N->NAlt != 0);

  bool (*Alt[N->NAlt])(const char *);

  for (unsigned ANum = 0; ANum < N->NAlt; ++ANum)
    Alt[ANum] = __builtin_mix_call(parseAlternativeForMix, N->Alt + ANum);

  for (unsigned ANum = 0; ANum < N->NAlt; ++ANum)
    if (Alt[ANum](Str))
      return true;

  return false;
}

bool parse(struct Symbol *S, const char *Str) {
  switch (S->T) {
  case T_Terminal:
    return parseTerminal((struct Terminal *)S->Node, Str);

  case T_Nonterminal:
    return parseNonterminal((struct Nonterminal *)S->Node, Str);
  }
}

static __stage(1) bool parseForMix(struct Symbol *S,
                                   __stage(1) const char *Str) __stage(1) {
  switch (S->T) {
  case T_Terminal:
    return parseTerminal((struct Terminal *)S->Node, Str);

  case T_Nonterminal:
    return parseNonterminalForMix((struct Nonterminal *)S->Node, Str);
  }
}

__attribute__((mix(parseForMix))) void *mixParse(void *, struct Symbol *);
