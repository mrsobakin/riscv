#include "memory.h"

#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "debug.h"


void _cache_bit_plru_try_reset(struct cache* c, uint32_t index) {
    uint8_t i = 0;

    for (i = 0; i < CACHE_WAY; ++i) {
        if (c->time[index][i] == 0) {
            return;
        }
    }

    for (i = 0; i < CACHE_WAY; ++i) {
        c->time[index][i] = 0;
    }
}

uint8_t _cache_find_victim_way(struct cache* c, uint32_t index) {
    uint8_t i = 0;

    if (c->rpolicy == CACHE_LRU) {
        uint8_t imin = 0;
        uint32_t tmin = c->time[index][0];
        
        for (i = 1; i < CACHE_WAY; ++i) {
            if (c->time[index][i] < tmin) {
                imin = i;
                tmin = c->time[index][i];
            }
        }

        return imin;
    }

    // bit p-LRU
    for (i = 0; i < CACHE_WAY; ++i) {
        if (c->time[index][i] == 0) {
            return i;
        }
    }

    PANIC("No victim way found for bit p-LRU");
}

void _cache_update_time(struct cache* c, uint32_t index, uint8_t way) {
    if (c->rpolicy == CACHE_LRU) {
        c->time[index][way] = ++(c->t);
    } else if (c->rpolicy == CACHE_BIT_PLRU) {
        c->time[index][way] = 1;
        _cache_bit_plru_try_reset(c, index);
        c->time[index][way] = 1;
    }
}

bool _cache_access(struct cache* c, struct memory* m, union _cache_addr addr, uint8_t** line) {
    uint8_t i;

    for (i = 0; i < CACHE_WAY; ++i) {
        if (c->tags[addr.parts.index][i] == addr.parts.tag) {
            _cache_update_time(c, addr.parts.index, i);
            *line = c->cache[addr.parts.index][i];
            return true;
        }
    }

    uint8_t way = _cache_find_victim_way(c, addr.parts.index);

    // Write cache back into memory, if it is even valid
    if (c->tags[addr.parts.index][way] != UINT32_MAX) {
        uint32_t old_addr = (union _cache_addr) {
            .parts = {
                .tag = c->tags[addr.parts.index][way],
                .index = addr.parts.index,
                .offset = 0,
            }
        }.raw;
        memcpy(m->mem + old_addr, c->cache[addr.parts.index][way], CACHE_LINE_SIZE);
    }

    // Copy memory block into cache line
    addr.parts.offset = 0;
    memcpy(c->cache[addr.parts.index][way], m->mem + addr.raw, CACHE_LINE_SIZE);
    c->tags[addr.parts.index][way] = addr.parts.tag;
    _cache_update_time(c, addr.parts.index, way);

    *line = c->cache[addr.parts.index][way];

    return false;
}


bool cache_read(struct cache* c, struct memory* m, uint32_t addr, uint8_t* val) {
    if(addr > MEM_SIZE)
        PANIC("Attempting to read from address outside of memory: %i", addr);

    union _cache_addr a = { .raw = addr };
    
    uint8_t* line;
    bool hit = _cache_access(c, m, a, &line);

    debug("cache r %i %s\n", addr, hit ? "hit" : "miss");

    *val = line[a.parts.offset];

    return hit;
}

bool cache_write(struct cache* c, struct memory* m, uint32_t addr, uint8_t val) {
    if(addr > MEM_SIZE)
        PANIC("Attempting to write to address outside of memory: %i = %i", addr, val);

    union _cache_addr a = { .raw = addr };
    
    uint8_t* line;
    bool hit = _cache_access(c, m, a, &line);

    debug("cache w %i %i %s\n", addr, val, hit ? "hit" : "miss");

    line[a.parts.offset] = val;

    return hit;
}

uint32_t mem_read8(struct memory* m, uint32_t addr) {
    debug("memread8 @ %i\n", addr);

    ++m->total;

    uint8_t a;
    if (cache_read(&m->cache, m, addr, &a)) {
        ++m->hits;
    }

    return a;
}

uint32_t mem_read16(struct memory* m, uint32_t addr) {
    debug("memread16 @ %i\n", addr);

    ++m->total;

    uint8_t a, b;
    if (cache_read(&m->cache, m, addr, &a) &
        cache_read(&m->cache, m, addr + 1, &b)) {
        ++m->hits;
    }

    return
        (uint32_t)a |
        (uint32_t)b << 8;
}

uint32_t mem_read32(struct memory* m, uint32_t addr) {
    debug("memread32 @ %i\n", addr);

    ++m->total;

    uint8_t a, b, c, d;
    if (cache_read(&m->cache, m, addr, &a) &
        cache_read(&m->cache, m, addr + 1, &b) &
        cache_read(&m->cache, m, addr + 2, &c) &
        cache_read(&m->cache, m, addr + 3, &d)) {
        ++m->hits;
    }

    return 
        (uint32_t)a       |
        (uint32_t)b << 8  |
        (uint32_t)c << 16 |
        (uint32_t)d << 24;
}

void mem_write8(struct memory* m, uint32_t addr, uint32_t val) {
    debug("memwrite8 @ %i = %i\n", addr, val);

    ++m->total;
    if (cache_write(&m->cache, m, addr, val)) {
        ++m->hits;
    }
}

void mem_write16(struct memory* m, uint32_t addr, uint32_t val) {
    debug("memwrite16 @ %i = %i\n", addr, val);

    ++m->total;
    if (cache_write(&m->cache, m, addr, val) &
        cache_write(&m->cache, m, addr + 1, val >> 8)) {
        ++m->hits;
    }
}

void mem_write32(struct memory* m, uint32_t addr, uint32_t val) {
    debug("memwrite32 @ %i = %i\n", addr, val);

    ++m->total;
    if (cache_write(&m->cache, m, addr, val) &
        cache_write(&m->cache, m, addr + 1, val >> 8) &
        cache_write(&m->cache, m, addr + 2, val >> 16) &
        cache_write(&m->cache, m, addr + 3, val >> 24)) {
        ++m->hits;
    }
}
