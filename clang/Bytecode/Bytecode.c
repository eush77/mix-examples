#include "Bytecode.h"

static int Regs[5];

__stage(1) int eval(struct Instruction *PC, __stage(1) int *Args) __stage(1) {
  switch (PC->Op) {
  case O_Int:
    Regs[PC->Operands[0]] = PC->Operands[1];
    return eval(PC + 1, Args);

  case O_Par:
    Regs[PC->Operands[0]] = Args[PC->Operands[1]];
    return eval(PC + 1, Args);

  case O_Mov:
    Regs[PC->Operands[0]] = Regs[PC->Operands[1]];
    return eval(PC + 1, Args);

  case O_Add:
    Regs[PC->Operands[0]] += Regs[PC->Operands[1]];
    return eval(PC + 1, Args);

  case O_Sub:
    Regs[PC->Operands[0]] -= Regs[PC->Operands[1]];
    return eval(PC + 1, Args);

  case O_Jmp:
    return eval(PC + PC->Operands[0], Args);

  case O_Jze:
    if (Regs[PC->Operands[0]])
      return eval(PC + PC->Operands[1], Args);
    else
      return eval(PC + 1, Args);

  case O_Ret:
    return Regs[PC->Operands[0]];
  }
}

__attribute__((mix(eval))) void *mixEval(void *, struct Instruction *);
