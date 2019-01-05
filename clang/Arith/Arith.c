#include "Arith.h"

__stage(1) int eval(struct Node *Expr, __stage(1) int *Args) __stage(1) {
  struct BinaryNode *BExpr = (struct BinaryNode *)Expr;

  switch (Expr->Op) {
  case O_Int:
    return ((struct IntNode *)Expr)->Value;

  case O_Param:
    return Args[((struct ParamNode *)Expr)->Number];

  case O_Add:
    return eval(BExpr->Left, Args) + eval(BExpr->Right, Args);

  case O_Sub:
    return eval(BExpr->Left, Args) - eval(BExpr->Right, Args);

  case O_Mul:
    return eval(BExpr->Left, Args) * eval(BExpr->Right, Args);

  case O_Div:
    return eval(BExpr->Left, Args) / eval(BExpr->Right, Args);

  case O_Rem:
    return eval(BExpr->Left, Args) % eval(BExpr->Right, Args);
  }
}

__attribute__((mix_ir(eval))) void *mixEval(void *, struct Node *);
