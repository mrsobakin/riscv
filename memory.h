#ifndef _CACHE_H
#define _CACHE_H

#include <stdint.h>
#include <stdbool.h>

#include "core.h"
#include "config.h"


enum cache_replacement_policy {
    CACHE_LRU,
    CACHE_BIT_PLRU,
};

struct cache {
    uint32_t t;
    uint32_t time[CACHE_SETS][CACHE_WAY];
    uint32_t tags[CACHE_SETS][CACHE_WAY];
    uint8_t cache[CACHE_SETS][CACHE_WAY][CACHE_LINE_SIZE];
    enum cache_replacement_policy rpolicy;
};

struct memory {
    uint8_t mem[MEM_SIZE];
    struct cache cache;
    size_t hits;
    size_t total;
};

union _cache_addr {
    uint32_t raw;
    struct {
        uint32_t offset : CACHE_OFFSET_LEN;
        uint32_t index  : CACHE_INDEX_LEN;
        uint32_t tag    : CACHE_TAG_LEN;
        uint32_t junk   : 32 - ADDR_LEN;
    } parts;
};

void _cache_bit_plru_try_reset(struct cache* c, uint32_t index);
uint8_t _cache_find_victim_way(struct cache* c, uint32_t index);
bool _cache_access(struct cache* c, struct memory* m, union _cache_addr addr, uint8_t** line);

bool cache_read(struct cache* c, struct memory* m, uint32_t addr, uint8_t* val);
bool cache_write(struct cache* c, struct memory* m, uint32_t addr, uint8_t val);

uint32_t mem_read8(struct memory* m, uint32_t addr);
uint32_t mem_read16(struct memory* m, uint32_t addr);
uint32_t mem_read32(struct memory* m, uint32_t addr);

void mem_write8(struct memory* m, uint32_t addr, uint32_t val);
void mem_write16(struct memory* m, uint32_t addr, uint32_t val);
void mem_write32(struct memory* m, uint32_t addr, uint32_t val);

#endif // _CACHE_H
