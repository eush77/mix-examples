#include "RecursiveDescent.h"

#include <stddef.h>

static __stage(1) const
    char *parseTerminal(struct Terminal *T,
                        __stage(1) const char *Str) __stage(1) {
  return *Str == T->Ch ? Str + 1 : NULL;
}

static __stage(1) const
    char *parseNonterminal(struct Nonterminal *N,
                           __stage(1) const char *Str) __stage(1);

static __stage(1) const
    char *parse(struct Symbol *S, __stage(1) const char *Str) __stage(1) {
  switch (S->T) {
  case T_Terminal:
    return parseTerminal((struct Terminal *)S->Node, Str);

  case T_Nonterminal:
    return parseNonterminal((struct Nonterminal *)S->Node, Str);
  }
}

__attribute__((mix(parse))) void *mixParse(void *, struct Symbol *);

static __stage(1) const
    char *parseAlternative(struct Alternative *A,
                           __stage(1) const char *Str) __stage(1) {
  __builtin_assume(A->NSym != 0);

  const char *(*Sym[A->NSym])(const char *);

  for (unsigned SNum = 0; SNum < A->NSym; ++SNum)
    Sym[SNum] = __builtin_mix_call(parse, A->Sym + SNum);

  for (unsigned SNum = 0; SNum < A->NSym; ++SNum) {
    if (!(Str = Sym[SNum](Str)))
      return NULL;
  }
  return Str;
}

static __stage(1) const
    char *parseNonterminal(struct Nonterminal *N,
                           __stage(1) const char *Str) __stage(1) {
  __builtin_assume(N->NAlt != 0);

  const char *(*Alt[N->NAlt])(const char *);

  for (unsigned ANum = 0; ANum < N->NAlt; ++ANum)
    Alt[ANum] = __builtin_mix_call(parseAlternative, N->Alt + ANum);

  for (unsigned ANum = 0; ANum < N->NAlt; ++ANum) {
    const char *End = Alt[ANum](Str);

    if (End)
      return End;
  }

  return NULL;
}
