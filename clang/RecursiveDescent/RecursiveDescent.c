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

static __stage(1) const
    char *parseSymbol(struct Symbol *S, __stage(1) const char *Str) __stage(1) {
  switch (S->T) {
  case T_Terminal:
    return parseTerminal((struct Terminal *)S->Node, Str);

  case T_Nonterminal:
    return parseNonterminal((struct Nonterminal *)S->Node, Str);
  }
}

static __stage(1) const
    char *parse(unsigned NumSymbols, struct Symbol Symbols[],
                unsigned NumAlternatives, struct Alternative Alternatives[],
                struct Symbol *Start, __stage(1) const char *Str) __stage(1) {
  for (struct Symbol *S = Symbols; S != Symbols + NumSymbols; ++S)
    S->Parse = __builtin_mix_call(parseSymbol, S);

  for (struct Alternative *A = Alternatives;
       A != Alternatives + NumAlternatives; ++A)
    A->Parse = __builtin_mix_call(parseAlternative, A);

  return Start->Parse(Str);
}

__attribute__((mix(parse))) void *mixParse(void *Ctx, unsigned NumSymbols,
                                           struct Symbol Symbols[],
                                           unsigned NumAlternatives,
                                           struct Alternative Alternatives[],
                                           struct Symbol *Start);
