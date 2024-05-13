#ifndef _MACHINE_H
#define _MACHINE_H

#include <stdint.h>

#include "config.h"
#include "core.h"
#include "memory.h"

struct machine {
    uint32_t pc;
    struct memory mem;
    uint32_t rs[32];
};

int32_t sext20(uint32_t x);
int32_t sext12(uint32_t x);
int32_t sext8(uint32_t x);
int32_t sext16(uint32_t x);
int32_t sext32(uint32_t x);

struct machine machine_new(enum cache_replacement_policy policy);

void machine_process_ir(struct machine* m, struct intermediate ir);

void machine_debug_print(struct machine* m);

#endif // _MACHINE_H
