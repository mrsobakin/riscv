#include "instructions.h"

#include "panic.h"


#define GEN_ARR(h, n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = n_args,
const uint8_t __INSTR_N_ARGS[] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

#define GEN_ARR(h, n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = opcode,
const uint8_t __INSTR_OPCODE[] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

#define GEN_ARR(h, n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = func3,
const uint8_t __INSTR_FUNC3[] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

#define GEN_ARR(h, n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = func7,
const uint8_t __INSTR_FUNC7[] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

#define GEN_ARR(h, n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = args,
const char __INSTR_ARGS_TYPES[][3] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

enum instr parse_instr(const char* str) {
#define GEN_IF_STRCMP(h, nargs, args, str_, ident, opcode, func3, func7) \
    if (strcmp(str_, str) == 0) \
        return INSTR_##ident;

    FOREACH_INSTR(GEN_IF_STRCMP)
#undef GEN_IF_STRCMP

    PANIC("No such instruction: %s", str);
}

inline uint8_t instr_opcode(enum instr instr) {
    return __INSTR_OPCODE[instr];
}

inline uint8_t instr_func3(enum instr instr) {
    return __INSTR_FUNC3[instr];
}

inline uint8_t instr_func7(enum instr instr) {
    return __INSTR_FUNC7[instr];
}

inline uint8_t instr_n_args(enum instr instr) {
    return __INSTR_N_ARGS[instr];
}

inline const char* instr_args_types(enum instr instr) {
    return __INSTR_ARGS_TYPES[instr];
}

inline uint8_t bin_opcode(uint32_t bin) {
    return bin & 0b1111111;
}

enum instr_fmt_type opcode_fmt_type(uint8_t opcode) {
    switch (opcode) {
        case 0b0110111: // lui
            return instr_fmt_u;
        case 0b0010111: // auipc
            return instr_fmt_u;
        case 0b1101111: // jal
            return instr_fmt_j;
        case 0b1100111: // jalr
            return instr_fmt_i;
        case 0b1100011: // beq, bne, blt, bge, bltu, bgeu
            return instr_fmt_b;
        case 0b0000011: // lb, lh, lw, lbu, lhu
            return instr_fmt_i;
        case 0b0100011: // sb, sh, sw
            return instr_fmt_s;
        case 0b0010011: // addi, slti, sltiu, xori, ori, andi, slli, srli, srai
            return instr_fmt_i;
        case 0b0110011: // add, sub, sll, slt, sltu, xor, srl, sra, or, and, mul, mulh, mulhsu, mulhu, div, divu, rem, remu
            return instr_fmt_r;
        case 0b0001111: // pause, fence, fence.i
            return instr_fmt_i;
        case 0b1110011: // ecall, ebreak
            return instr_fmt_i;
        default:
            PANIC("Unknown opcode: %i", opcode);
    }
}
