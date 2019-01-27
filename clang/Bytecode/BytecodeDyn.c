#include "Bytecode.h"

static int Regs[5];

__stage(1) int evalDyn(unsigned ProgramSize, struct Instruction *PC,
                       __stage(1) int *Args) __stage(1) {
  for (;;) {
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
      PC += PC->Operands[0];
      continue;

    case O_Jze:
      if (Regs[PC->Operands[0]])
        break;
      PC += PC->Operands[1];
      continue;

    case O_Ret:
      return Regs[PC->Operands[0]];
    }

    PC += 1;
  }
}

__attribute__((mix(evalDyn))) void *mixEvalDyn(void *, unsigned,
                                               struct Instruction *);
