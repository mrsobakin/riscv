#include "instructions.h"

#include "panic.h"


#define GEN_ARR(n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = n_args,
const uint8_t __INSTR_N_ARGS[] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

#define GEN_ARR(n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = opcode,
const uint8_t __INSTR_OPCODE[] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

#define GEN_ARR(n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = func3,
const uint8_t __INSTR_FUNC3[] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

#define GEN_ARR(n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = func7,
const uint8_t __INSTR_FUNC7[] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

#define GEN_ARR(n_args, args, str, ident, opcode, func3, func7) [INSTR_##ident] = args,
const char __INSTR_ARGS_TYPES[][3] = {
    FOREACH_INSTR(GEN_ARR)
};
#undef GEN_ARR

enum instr parse_instr(const char* str) {
#define GEN_IF_STRCMP(nargs, args, str_, ident, opcode, func3, func7) \
    if (strcmp(str_, str) == 0) \
        return INSTR_##ident;

    FOREACH_INSTR(GEN_IF_STRCMP)
#undef GEN_IF_STRCMP

    PANIC("No such instruction: %s", str);
}

uint8_t instr_opcode(enum instr instr) {
    return __INSTR_OPCODE[instr];
}

uint8_t instr_func3(enum instr instr) {
    return __INSTR_FUNC3[instr];
}

uint8_t instr_func7(enum instr instr) {
    return __INSTR_FUNC7[instr];
}

uint8_t instr_n_args(enum instr instr) {
    return __INSTR_N_ARGS[instr];
}

const char* instr_args_types(enum instr instr) {
    return __INSTR_ARGS_TYPES[instr];
}
