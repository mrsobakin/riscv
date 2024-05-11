#ifndef _CONFIG_H
#define _CONFIG_H

#define MEM_SIZE            512 * 1024
#define CACHE_SIZE          2 * 1024
#define CACHE_LINE_SIZE     32          // 2**CACHE_OFFSET_LEN
#define CACHE_LINE_COUNT    64          // CACHE_SIZE / CACHE_LINE_SIZE
#define CACHE_WAY           4           // CACHE_LINE_COUNT / CACHE_SETS
#define CACHE_SETS          16          // 2**CACHE_INDEX_LEN
#define ADDR_LEN            19          // log2(MEM_SIZE)
#define CACHE_TAG_LEN       10          // ADDR_LEN - CACHE_INDEX_LEN - CACHE_OFFSET_LEN
#define CACHE_INDEX_LEN     4
#define CACHE_OFFSET_LEN    5

#endif // _CONFIG_H
