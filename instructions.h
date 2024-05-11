#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "panic.h"

#include <endian.h>
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#   error "Only little endian systems are supported for now"
#endif

#define FOREACH_INSTR(M) \
    /* n, args    str       ident      opcode      func3   func7     */ \
    M( 2, "rn ", "lui",     LUI,       0b0110111,  0,      0          ) \
    M( 2, "rn ", "auipc",   AUIPC,     0b0010111,  0,      0          ) \
    M( 2, "rn ", "jal",     JAL,       0b1101111,  0,      0          ) \
    M( 3, "rnr", "jalr",    JALR,      0b1100111,  0,      0          ) \
    M( 3, "rrn", "beq",     BEQ,       0b1100011,  0b000,  0          ) \
    M( 3, "rrn", "bne",     BNE,       0b1100011,  0b001,  0          ) \
    M( 3, "rrn", "blt",     BLT,       0b1100011,  0b100,  0          ) \
    M( 3, "rrn", "bge",     BGE,       0b1100011,  0b101,  0          ) \
    M( 3, "rrn", "bltu",    BLTU,      0b1100011,  0b110,  0          ) \
    M( 3, "rrn", "bgeu",    BGEU,      0b1100011,  0b111,  0          ) \
    M( 3, "rnr", "lb",      LB,        0b0000011,  0b000,  0          ) \
    M( 3, "rnr", "lh",      LH,        0b0000011,  0b001,  0          ) \
    M( 3, "rnr", "lw",      LW,        0b0000011,  0b010,  0          ) \
    M( 3, "rnr", "lbu",     LBU,       0b0000011,  0b100,  0          ) \
    M( 3, "rnr", "lhu",     LHU,       0b0000011,  0b101,  0          ) \
    M( 3, "rnr", "sb",      SB,        0b0100011,  0b000,  0          ) \
    M( 3, "rnr", "sh",      SH,        0b0100011,  0b001,  0          ) \
    M( 3, "rnr", "sw",      SW,        0b0100011,  0b010,  0          ) \
    M( 3, "rrn", "addi",    ADDI,      0b0010011,  0b000,  0          ) \
    M( 3, "rrn", "slti",    SLTI,      0b0010011,  0b010,  0          ) \
    M( 3, "rrn", "sltiu",   SLTIU,     0b0010011,  0b011,  0          ) \
    M( 3, "rrn", "xori",    XORI,      0b0010011,  0b100,  0          ) \
    M( 3, "rrn", "ori",     ORI,       0b0010011,  0b110,  0          ) \
    M( 3, "rrn", "andi",    ANDI,      0b0010011,  0b111,  0          ) \
    M( 3, "rrn", "slli",    SLLI,      0b0010011,  0b001,  0b0000000  ) \
    M( 3, "rrn", "srli",    SRLI,      0b0010011,  0b101,  0b0000000  ) \
    M( 3, "rrn", "srai",    SRAI,      0b0010011,  0b101,  0b0100000  ) \
    M( 3, "rrr", "add",     ADD,       0b0110011,  0b000,  0b0000000  ) \
    M( 3, "rrr", "sub",     SUB,       0b0110011,  0b000,  0b0100000  ) \
    M( 3, "rrr", "sll",     SLL,       0b0110011,  0b001,  0b0000000  ) \
    M( 3, "rrr", "slt",     SLT,       0b0110011,  0b010,  0b0000000  ) \
    M( 3, "rrr", "sltu",    SLTU,      0b0110011,  0b011,  0b0000000  ) \
    M( 3, "rrr", "xor",     XOR,       0b0110011,  0b100,  0b0000000  ) \
    M( 3, "rrr", "srl",     SRL,       0b0110011,  0b101,  0b0000000  ) \
    M( 3, "rrr", "sra",     SRA,       0b0110011,  0b101,  0b0100000  ) \
    M( 3, "rrr", "or",      OR,        0b0110011,  0b110,  0b0000000  ) \
    M( 3, "rrr", "and",     AND,       0b0110011,  0b111,  0b0000000  ) \
    M( 2, "   ", "fence",   FENCE,     0b0001111,  0b000,  0          ) \
    M( 0, "   ", "fence.tso", FENCE_TSO, 0b0001111,  0b000,  0          ) \
    M( 0, "   ", "pause",   PAUSE,     0b0001111,  0b000,  0          ) \
    M( 0, "   ", "ecall",   ECALL,     0b1110011,  0b000,  0          ) \
    M( 0, "   ", "ebreak",  EBREAK,    0b1110011,  0b000,  0          ) \
    M( 3, "rrr", "mul",     MUL,       0b0110011,  0b000,  0b0000001  ) \
    M( 3, "rrr", "mulh",    MULH,      0b0110011,  0b001,  0b0000001  ) \
    M( 3, "rrr", "mulhsu",  MULHSU,    0b0110011,  0b010,  0b0000001  ) \
    M( 3, "rrr", "mulhu",   MULHU,     0b0110011,  0b011,  0b0000001  ) \
    M( 3, "rrr", "div",     DIV,       0b0110011,  0b100,  0b0000001  ) \
    M( 3, "rrr", "divu",    DIVU,      0b0110011,  0b101,  0b0000001  ) \
    M( 3, "rrr", "rem",     REM,       0b0110011,  0b110,  0b0000001  ) \
    M( 3, "rrr", "remu",    REMU,      0b0110011,  0b111,  0b0000001  )

enum instr_fmt_type {
    instr_fmt_r,
    instr_fmt_i,
    instr_fmt_s,
    instr_fmt_b,
    instr_fmt_u,
    instr_fmt_j,
};

struct instr_r {
    uint32_t opcode : 7;
    uint32_t rd     : 5;
    uint32_t funct3 : 3;
    uint32_t rs1    : 5;
    uint32_t rs2    : 5;
    uint32_t funct7 : 7;
};

struct instr_i {
    uint32_t opcode : 7;
    uint32_t rd     : 5;
    uint32_t funct3 : 3;
    uint32_t rs1    : 5;
    uint32_t imm    : 12;
};

struct instr_s {
    uint32_t opcode   : 7;
    uint32_t imm4_0   : 5;
    uint32_t funct3   : 3;
    uint32_t rs1      : 5;
    uint32_t rs2      : 5;
    uint32_t imm11_5  : 7;
};

struct instr_b {
    uint32_t opcode   : 7;
    uint32_t imm11    : 1;
    uint32_t imm4_1   : 4;
    uint32_t funct3   : 3;
    uint32_t rs1      : 5;
    uint32_t rs2      : 5;
    uint32_t imm10_5  : 6;
    uint32_t imm12    : 1;
};

struct instr_u {
    uint32_t opcode : 7;
    uint32_t rd     : 5;
    uint32_t imm    : 20;
};

struct instr_j {
    uint32_t opcode   : 7;
    uint32_t rd       : 5;
    uint32_t imm19_12 : 8;
    uint32_t imm11    : 1;
    uint32_t imm10_1  : 10;
    uint32_t imm20    : 1;
};

union instr_generic {
    uint32_t raw;
    struct instr_r r;
    struct instr_i i;
    struct instr_s s;
    struct instr_b b;
    struct instr_u u;
    struct instr_j j;
};

#define GEN_ENUM_ELEMENT(nargs, args, str, ident, opcode, func3, func7) INSTR_##ident,
enum instr {
    FOREACH_INSTR(GEN_ENUM_ELEMENT)
};
#undef ENUM_ELEMENT


extern const uint8_t __INSTR_N_ARGS[];
extern const uint8_t __INSTR_OPCODE[];
extern const uint8_t __INSTR_FUNC3[];
extern const uint8_t __INSTR_FUNC7[];
extern const char    __INSTR_ARGS_TYPES[][3];

uint8_t instr_opcode(enum instr instr);
uint8_t instr_func3(enum instr instr);
uint8_t instr_func7(enum instr instr);
uint8_t instr_n_args(enum instr instr);
const char* instr_args_types(enum instr instr);

enum instr parse_instr(const char* str);

#endif // _INSTRUCTIONS_H
