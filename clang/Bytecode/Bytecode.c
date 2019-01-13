#include "Bytecode.h"

__stage(1) int eval(struct Instruction *Program,
                    __stage(1) int *Args) __stage(1) {
  int Regs[5];

  for (struct Instruction *PC = Program;; ++PC) {
    switch (PC->Op) {
    case O_Int:
      Regs[PC->Operands[0]] = PC->Operands[1];
      break;

    case O_Par:
      Regs[PC->Operands[0]] = Args[PC->Operands[1]];
      break;

    case O_Mov:
      Regs[PC->Operands[0]] = Regs[PC->Operands[1]];
      break;

    case O_Add:
      Regs[PC->Operands[0]] += Regs[PC->Operands[1]];
      break;

    case O_Sub:
      Regs[PC->Operands[0]] -= Regs[PC->Operands[1]];
      break;

    case O_Jmp:
      PC = Program + PC->Operands[0];
      continue;

    case O_Jze:
      if (Regs[PC->Operands[0]])
        break;
      PC = Program + PC->Operands[1];
      continue;

    case O_Ret:
      return Regs[PC->Operands[0]];
    }
  }
}

__attribute__((mix(eval))) void *mixEval(void *, struct Instruction *);
