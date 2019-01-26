#ifndef MIX_EXAMPLES_CLANG_RECURSIVEDESCENT_RECURSIVEDESCENT_H
#define MIX_EXAMPLES_CLANG_RECURSIVEDESCENT_RECURSIVEDESCENT_H

#ifdef __cplusplus
extern "C" {
#define __stage(_)
#endif

enum Tag { T_Terminal, T_Nonterminal };

struct Symbol {
  enum Tag T;
  void *Node;
  const char *(*Parse)(const char *) __stage(2);
} __attribute__((staged));

struct Terminal {
  char Ch;
} __attribute__((staged));

struct Alternative {
  unsigned NSym;
  struct Symbol *Sym;
  const char *(*Parse)(const char *) __stage(2);
} __attribute__((staged));

struct Nonterminal {
  unsigned NAlt;
  struct Alternative *Alt;
} __attribute__((staged));

const char *parseInt(struct Symbol *, const char *);
void *mixParse(void *Ctx, unsigned NumSymbols, struct Symbol Symbols[],
               unsigned NumAlternatives, struct Alternative Alternatives[],
               struct Symbol *Start);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MIX_EXAMPLES_CLANG_RECURSIVEDESCENT_RECURSIVEDESCENT_H
