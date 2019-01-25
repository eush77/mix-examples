#include "RecursiveDescent.h"

#include <stddef.h>

static __stage(1) const
    char *parseTerminal(struct Terminal *T,
                        __stage(1) const char *Str) __stage(1) {
  return *Str == T->Ch ? Str + 1 : NULL;
}

static const char *parseNonterminal(struct Nonterminal *N, const char *Str);
static __stage(1) const
    char *parseNonterminalForMix(struct Nonterminal *N,
                                 __stage(1) const char *Str) __stage(1);

const char *parse(struct Symbol *S, const char *Str) {
  switch (S->T) {
  case T_Terminal:
    return parseTerminal((struct Terminal *)S->Node, Str);

  case T_Nonterminal:
    return parseNonterminal((struct Nonterminal *)S->Node, Str);
  }
}

static __stage(1) const
    char *parseForMix(struct Symbol *S, __stage(1) const char *Str) __stage(1) {
  switch (S->T) {
  case T_Terminal:
    return parseTerminal((struct Terminal *)S->Node, Str);

  case T_Nonterminal:
    return parseNonterminalForMix((struct Nonterminal *)S->Node, Str);
  }
}

__attribute__((mix(parseForMix))) void *mixParse(void *, struct Symbol *);

static __stage(1) const
    char *parseAlternative(struct Alternative *A,
                           __stage(1) const char *Str) __stage(1) {
  for (struct Symbol *S = A->Sym; S != A->Sym + A->NSym; ++S) {
    if (!(Str = parse(S, Str)))
      return NULL;
  }
  return Str;
}

static __stage(1) const
    char *parseAlternativeForMix(struct Alternative *A,
                                 __stage(1) const char *Str) __stage(1) {
  __builtin_assume(A->NSym != 0);

  const char *(*Sym[A->NSym])(const char *);

  for (unsigned SNum = 0; SNum < A->NSym; ++SNum)
    Sym[SNum] = __builtin_mix_call(parseForMix, A->Sym + SNum);

  for (unsigned SNum = 0; SNum < A->NSym; ++SNum) {
    if (!(Str = Sym[SNum](Str)))
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

static __stage(1) const
    char *parseNonterminalForMix(struct Nonterminal *N,
                                 __stage(1) const char *Str) __stage(1) {
  __builtin_assume(N->NAlt != 0);

  const char *(*Alt[N->NAlt])(const char *);


  for (unsigned ANum = 0; ANum < N->NAlt; ++ANum)
    Alt[ANum] = __builtin_mix_call(parseAlternativeForMix, N->Alt + ANum);

  for (unsigned ANum = 0; ANum < N->NAlt; ++ANum) {
    const char *End = Alt[ANum](Str);

    if (End)
      return End;
  }

  return NULL;
}
