#ifndef MIX_EXAMPLES_CLANG_ARITH_ARITH_H
#define MIX_EXAMPLES_CLANG_ARITH_ARITH_H

#ifdef __cplusplus
extern "C" {
#endif

enum Opcode {
  O_Int,
  O_Param,
  O_Add,
  O_Sub,
  O_Mul,
  O_Div,
  O_Rem,
};

struct Node {
  enum Opcode Op;
} __attribute__((staged));

struct IntNode {
  enum Opcode Op;
  int Value;
} __attribute__((staged));

struct ParamNode {
  enum Opcode Op;
  unsigned Number;
} __attribute__((staged));

struct BinaryNode {
  enum Opcode Op;
  struct Node *Left, *Right;
} __attribute__((staged));

int evalExpr(struct Node *, int *);
void *mixEvalExpr(void *Ctx, struct Node *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MIX_EXAMPLES_CLANG_ARITH_ARITH_H
