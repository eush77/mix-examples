#include "Arith.h"

__stage(1) int evalExpr(struct Node *Expr, __stage(1) int *Args) __stage(1) {
  struct BinaryNode *BExpr = (struct BinaryNode *)Expr;

  switch (Expr->Op) {
  case O_Int:
    return ((struct IntNode *)Expr)->Value;

  case O_Param:
    return Args[((struct ParamNode *)Expr)->Number];

  case O_Add:
    return evalExpr(BExpr->Left, Args) + evalExpr(BExpr->Right, Args);

  case O_Sub:
    return evalExpr(BExpr->Left, Args) - evalExpr(BExpr->Right, Args);

  case O_Mul:
    return evalExpr(BExpr->Left, Args) * evalExpr(BExpr->Right, Args);

  case O_Div:
    return evalExpr(BExpr->Left, Args) / evalExpr(BExpr->Right, Args);

  case O_Rem:
    return evalExpr(BExpr->Left, Args) % evalExpr(BExpr->Right, Args);
  }
}

__attribute__((mix(evalExpr))) void *mixEvalExpr(void *, struct Node *);
