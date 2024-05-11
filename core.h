#ifndef _CORE_H
#define _CORE_H

#include <stdint.h>

#include "instructions.h"

struct intermediate {
    enum instr instr;
    int32_t a;
    int32_t b;
    int32_t c;
};

uint32_t ir_to_bin(struct intermediate ir);

#endif // _CORE_H
