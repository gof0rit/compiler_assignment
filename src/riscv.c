#include "riscv.h"
#include <assert.h>
#include <stdbool.h>

RegInfo reg_info[32] = {
  {0, "zero", RA_RESERVED, false},
  {1, "ra", RA_RESERVED, false},
  {2, "sp", RA_RESERVED, false},
  {3, "gp", RA_RESERVED, false},
  {4, "tp", RA_RESERVED, false},
  {5, "t0", RA_TEMP, true},
  {6, "t1", RA_TEMP, true},
  {7, "t2", RA_TEMP, true},
  {8, "fp", RA_RESERVED, false},
  {9, "s1", RA_SAVE, true},
  {10, "a0", RA_ARG | RA_RET, true},
  {11, "a1", RA_ARG, true},
  {12, "a2", RA_ARG, true},
  {13, "a3", RA_ARG, true},
  {14, "a4", RA_ARG, true},
  {15, "a5", RA_ARG, true},
  {16, "a6", RA_ARG, true},
  {17, "a7", RA_ARG, true},
  {18, "s2", RA_SAVE, true},
  {19, "s3", RA_SAVE, true},
  {20, "s4", RA_SAVE, true},
  {21, "s5", RA_SAVE, true},
  {22, "s6", RA_SAVE, true},
  {23, "s7", RA_SAVE, true},
  {24, "s8", RA_SAVE, true},
  {25, "s9", RA_SAVE, true},
  {26, "s10", RA_SAVE, true},
  {27, "s11", RA_SAVE, true},
  {28, "t3", RA_TEMP, true},
  {29, "t4", RA_TEMP, true},
  {30, "t5", RA_TEMP, true},
  {31, "t6", RA_TEMP, true},
};

static const InstrInfo instr_info[] = {
  {OPC_LUI, "lui", "%R, %A"},
  {OPC_AUIPC, "auipc", "%R, %R"},
  {OPC_JAL, "jal", "%R, %R"},
  {OPC_JALR, "jalr", "%R, %R"},
  {OPC_BEQ, "beq", "%R, %R, %s"},
  {OPC_BNE, "bne", "%R, %R, %s"},
  {OPC_BLT, "blt", "%R, %R, %s"},
  {OPC_BGE, "bge", "%R, %R, %s"},
  {OPC_BLTU, "bltu", "%R, %R, %s"},
  {OPC_BGEU, "bgeu", "%R, %R, %s"},
  {OPC_LB, "lb", "%R, %A(%R)"},
  {OPC_LH, "lh", "%R, %A(%R)"},
  {OPC_LW, "lw", "%R, %A(%R)"},
  {OPC_LBU, "lbu", "%R, %A(%R)"},
  {OPC_LHU, "lhu", "%R, %A(%R)"},
  {OPC_SB, "sb", "%R, %A(%R)"},
  {OPC_SH, "sh", "%R, %A(%R)"},
  {OPC_SW, "sw", "%R, %A(%R)"},
  {OPC_ADDI, "addi", "%R, %R, %d"},
  {OPC_ADDI_A, "addi", "%R, %R, %A"},
  {OPC_SLTI, "slti", "%R, %R, %d"},
  {OPC_SLTIU, "sltiu", "%R, %R, %d"},
  {OPC_XORI, "xori", "%R, %R, %d"},
  {OPC_ORI, "ori", "%R, %R, %d"},
  {OPC_ANDI, "andi", "%R, %R, %d"},
  {OPC_SLLI, "slli", "%R, %R, %d"},
  {OPC_SRLI, "srli", "%R, %R, %d"},
  {OPC_SRAI, "srai", "%R, %R, %d"},
  {OPC_ADD, "add", "%R, %R, %R"},
  {OPC_SUB, "sub", "%R, %R, %R"},
  {OPC_SLL, "sll", "%R, %R, %R"},
  {OPC_SLT, "slt", "%R, %R, %R"},
  {OPC_SLTU, "sltu", "%R, %R, %R"},
  {OPC_XOR, "xor", "%R, %R, %R"},
  {OPC_SRL, "srl", "%R, %R, %R"},
  {OPC_SRA, "sra", "%R, %R, %R"},
  {OPC_OR, "or", "%R, %R, %R"},
  {OPC_AND, "and", "%R, %R, %R"},
  /* M extension */
  {OPC_MUL, "mul", "%R, %R, %R"},
  {OPC_DIV, "div", "%R, %R, %R"},
  {OPC_REM, "rem", "%R, %R, %R"},
  /* pseudo instructions */
  {OPC_CALL, "call", "%s"},
  {OPC_LI, "li", "%R, %d"},
  {OPC_MV, "mv", "%R, %R"},
  {OPC_J, "j", "%s"},
  {OPC_RET, "ret", ""},
};

int num_of_regs = sizeof(reg_info) / sizeof(RegInfo);

static int instr_info_size = sizeof(instr_info) / sizeof(InstrInfo);

const InstrInfo *get_instruction_info(int opc)
{
  assert(opc < instr_info_size);
  return &instr_info[opc];
}

