#ifndef PANDA_RISCV_H
#define PANDA_RISCV_H

#include <stdbool.h>

typedef enum {
  RA_GENERAL = 0,
  RA_RESERVED = 0x1,
  RA_ARG = 0x2,
  RA_RET = 0x4,
  RA_TEMP = 0x8,
  RA_SAVE = 0x10,
} RegAttr;

typedef enum {
  REG_ZERO,
  REG_RA,
  REG_SP,
  REG_GP,
  REG_TP,
  REG_T0,
  REG_T1,
  REG_T2,
  REG_FP,
  REG_S1,
  REG_A0,
  REG_A1,
  REG_A2,
  REG_A3,
  REG_A4,
  REG_A5,
  REG_A6,
  REG_A7,
  REG_S2,
  REG_S3,
  REG_S4,
  REG_S5,
  REG_S6,
  REG_S7,
  REG_S8,
  REG_S9,
  REG_S10,
  REG_S11,
  REG_T3,
  REG_T4,
  REG_T5,
  REG_T6,
} RegID;

typedef struct RegInfo {
  int id;
  char *name;
  RegAttr attr;
  bool available;
} RegInfo;

typedef enum {
  OPC_LUI,
  OPC_AUIPC,
  OPC_JAL,
  OPC_JALR,
  OPC_BEQ,
  OPC_BNE,
  OPC_BLT,
  OPC_BGE,
  OPC_BLTU,
  OPC_BGEU,
  OPC_LB,
  OPC_LH,
  OPC_LW,
  OPC_LBU,
  OPC_LHU,
  OPC_SB,
  OPC_SH,
  OPC_SW,
  OPC_ADDI,
  OPC_ADDI_A,
  OPC_SLTI,
  OPC_SLTIU,
  OPC_XORI,
  OPC_ORI,
  OPC_ANDI,
  OPC_SLLI,
  OPC_SRLI,
  OPC_SRAI,
  OPC_ADD,
  OPC_SUB,
  OPC_SLL,
  OPC_SLT,
  OPC_SLTU,
  OPC_XOR,
  OPC_SRL,
  OPC_SRA,
  OPC_OR,
  OPC_AND,
  OPC_MUL,
  OPC_DIV,
  OPC_REM,
  /* pseudo instructions */
  OPC_CALL,
  OPC_LI,
  OPC_MV,
  OPC_J,
  OPC_RET,
  NUM_OF_OPCODE
} Opcode;

typedef struct InstrInfo {
  Opcode opc;
  char *name;
  char *args;
} InstrInfo;

extern int num_of_regs;
extern RegInfo reg_info[32];
const InstrInfo *get_instruction_info(int opc);

#endif
