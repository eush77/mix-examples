#ifndef MIX_EXAMPLES_CLANG_RECURSIVEDESCENT_RECURSIVEDESCENT_H
#define MIX_EXAMPLES_CLANG_RECURSIVEDESCENT_RECURSIVEDESCENT_H

#ifdef __cplusplus
extern "C" {
#endif

enum Tag { T_Terminal, T_Nonterminal };

struct Symbol {
  enum Tag T;
  void *Node;
} __attribute__((staged));

struct Terminal {
  char Ch;
} __attribute__((staged));

struct Alternative {
  unsigned NSym;
  struct Symbol *Sym;
} __attribute__((staged));

struct Nonterminal {
  unsigned NAlt;
  struct Alternative *Alt;
} __attribute__((staged));

const char *parse(struct Symbol *, const char *);
void *mixParse(void *, struct Symbol *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MIX_EXAMPLES_CLANG_RECURSIVEDESCENT_RECURSIVEDESCENT_H
