#include "Bytecode.h"

static int Regs[5];

__attribute__((noinline)) static __stage(1) int id(__stage(1) int x) __stage(
    1) {
  return x;
}

static __stage(1) int evalBasicBlock(struct Instruction *PC,
                                     __stage(1) int *Args) __stage(1) {
  unsigned N = 0;

  while (PC->Op != O_Jmp && PC->Op != O_Jze && PC->Op != O_Ret) {
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

    default:
      __builtin_unreachable();
    }

    N += 1;
    PC += 1;
  }

  switch (PC->Op) {
  case O_Jmp:
    return N + PC->Operands[0];

  case O_Jze:
    return N + id(Regs[PC->Operands[0]] ? 1 : PC->Operands[1]);

  case O_Ret:
    return N;

  default:
    __builtin_unreachable();
  }
}

int evalOpt(unsigned ProgramSize, struct Instruction Program[ProgramSize],
            int *Args) {
  int PC = 0;
  int Delta;

  while ((Delta = evalBasicBlock(&Program[PC], Args)))
    PC += Delta;

  return Regs[Program[PC].Operands[0]];
}

static __stage(1) int evalForMix(unsigned ProgramSize,
                                 struct Instruction *Program,
                                 __stage(1) int *Args) __stage(1) {
  int (*Funcs[ProgramSize])(int*);

  for (struct Instruction *PC = Program; PC != Program + ProgramSize; ++PC)
    Funcs[PC - Program] = __builtin_mix_call(evalBasicBlock, PC);

  int PC = 0;
  int Delta;

  while ((Delta = Funcs[PC](Args)))
    PC += Delta;

  return Regs[Program[PC].Operands[0]];
}

__attribute__((mix(evalForMix))) void *mixEvalOpt(void *, unsigned,
                                                  struct Instruction *);
