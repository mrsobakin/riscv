#include "core.h"

#include <stdint.h>

#include "instructions.h"


uint32_t ir_to_bin(struct intermediate ir) {
    switch (ir.instr) {
        case INSTR_LUI:
        case INSTR_AUIPC:
            return (union instr_generic) {
                .u = {
                    .opcode = instr_opcode(ir.instr),
                    .rd = ir.a,
                    .imm = ir.b,
                }
            }.raw;

        case INSTR_JAL:
            return (union instr_generic) {
                .j = {
                    .opcode = instr_opcode(ir.instr),
                    .rd = ir.a,
                    .imm10_1  = ir.b >> 1,
                    .imm11    = ir.b >> 11,
                    .imm19_12 = ir.b >> 12,
                    .imm20    = ir.b >> 20,
                }
            }.raw;

        case INSTR_JALR:
            return (union instr_generic) {
                .i = {
                    .opcode = instr_opcode(ir.instr),
                    .rd = ir.a,
                    .funct3 = 0b000,
                    .rs1 = ir.c,
                    .imm = ir.b,
                }
            }.raw;

        case INSTR_BEQ:
        case INSTR_BNE:
        case INSTR_BLT:
        case INSTR_BGE:
        case INSTR_BLTU:
        case INSTR_BGEU:
            return (union instr_generic) {
                .b = {
                    .opcode = instr_opcode(ir.instr),
                    .rs1 = ir.a,
                    .rs2 = ir.b,
                    .funct3 = instr_func3(ir.instr),
                    .imm4_1  = ir.c >> 1,
                    .imm10_5 = ir.c >> 5,
                    .imm11   = ir.c >> 11,
                    .imm12   = ir.c >> 12,
                }
            }.raw;
            
        case INSTR_LB:
        case INSTR_LH:
        case INSTR_LW:
        case INSTR_LBU:
        case INSTR_LHU:
            return (union instr_generic) {
                .i = {
                    .opcode = instr_opcode(ir.instr),
                    .rd = ir.a,
                    .funct3 = instr_func3(ir.instr),
                    .rs1 = ir.c,
                    .imm = ir.b,
                }
            }.raw;

        case INSTR_SB:
        case INSTR_SH:
        case INSTR_SW:
            return (union instr_generic) {
                .s = {
                    .opcode = instr_opcode(ir.instr),
                    .rs1 = ir.c,
                    .rs2 = ir.a,
                    .funct3 = instr_func3(ir.instr),
                    .imm4_0 = ir.b,
                    .imm11_5 = ir.b >> 5,
                }
            }.raw;

        case INSTR_ADDI:
        case INSTR_SLTI:
        case INSTR_SLTIU:
        case INSTR_XORI:
        case INSTR_ORI:
        case INSTR_ANDI:
            return (union instr_generic) {
                .i = {
                    .opcode = instr_opcode(ir.instr),
                    .rd = ir.a,
                    .rs1 = ir.b,
                    .funct3 = instr_func3(ir.instr),
                    .imm = ir.c,
                }
            }.raw;

        case INSTR_SLLI:
        case INSTR_SRLI:
            return (union instr_generic) {
                .i = {
                    .opcode = instr_opcode(ir.instr),
                    .rd = ir.a,
                    .rs1 = ir.b,
                    .funct3 = instr_func3(ir.instr),
                    .imm = ir.c,
                }
            }.raw;

        case INSTR_SRAI:
            return (union instr_generic) {
                .i = {
                    .opcode = instr_opcode(ir.instr),
                    .rd = ir.a,
                    .rs1 = ir.b,
                    .funct3 = instr_func3(ir.instr),
                    .imm = 0b010000000000 | (ir.c & 0b11111),
                }
            }.raw;

        case INSTR_ADD:
        case INSTR_SUB:
        case INSTR_SLL:
        case INSTR_SLT:
        case INSTR_SLTU:
        case INSTR_XOR:
        case INSTR_SRL:
        case INSTR_SRA:
        case INSTR_OR:
        case INSTR_AND:
        case INSTR_MUL:
        case INSTR_MULH:
        case INSTR_MULHSU:
        case INSTR_MULHU:
        case INSTR_DIV:
        case INSTR_DIVU:
        case INSTR_REM:
        case INSTR_REMU:
            return (union instr_generic) {
                .r = {
                    .opcode = instr_opcode(ir.instr),
                    .rd = ir.a,
                    .rs1 = ir.b,
                    .rs2 = ir.c,
                    .funct3 = instr_func3(ir.instr),
                    .funct7 = instr_func7(ir.instr),
                }
            }.raw;

        case INSTR_FENCE:
            return (union instr_generic) {
                .i = {
                    .opcode = instr_opcode(ir.instr),
                    .imm = ir.b | ir.a << 4,
                }
            }.raw;
        case INSTR_FENCE_TSO:
            return 0x8330000F;
        case INSTR_ECALL:
            return 0x00000073;
        case INSTR_EBREAK:
            return 0x00100073;
        case INSTR_PAUSE:
            return 0x0100000F;
    }

    PANIC("Unknown instruction: %i", ir.instr);
}
