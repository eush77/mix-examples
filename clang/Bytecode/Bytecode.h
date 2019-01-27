#ifndef MIX_EXAMPLES_CLANG_BYTECODE_BYTECODE_H
#define MIX_EXAMPLES_CLANG_BYTECODE_BYTECODE_H

#ifdef __cplusplus
extern "C" {
#endif

enum Opcode {
  O_Int,
  O_Par,
  O_Mov,
  O_Add,
  O_Sub,
  O_Jmp,
  O_Jze,
  O_Ret,
};

struct Instruction {
  enum Opcode Op;
  int Operands[2];
} __attribute__((staged));

int evalDyn(unsigned ProgramSize, struct Instruction *Program, int *Args);
void *mixEvalDyn(void *Ctx, unsigned ProgramSize, struct Instruction *Program);
int eval(unsigned ProgramSize, struct Instruction *Program, int *Args);
void *mixEval(void *Ctx, unsigned ProgramSize, struct Instruction *Program);
int evalOpt(unsigned ProgramSize, struct Instruction *Program, int *Args);
void *mixEvalOpt(void *Ctx, unsigned ProgramSize, struct Instruction *Program);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MIX_EXAMPLES_CLANG_BYTECODE_BYTECODE_H
