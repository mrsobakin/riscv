#include <stdbool.h>
#include <inttypes.h>

#include "machine.h"
#include "panic.h"
#include "parser.h"


struct args {
    uint8_t repl_policy;
    const char* asm_path;
    const char* bin_path;
};

struct args parse_args(int argc, char* argv[]) {
    struct args args = {
        .repl_policy = UINT8_MAX,
        .asm_path = NULL,
        .bin_path = NULL,
    };

    char** arg = argv + 1;
    char** end = argv + argc;

    for (; arg < end; ++arg) {
        if (arg + 1 >= end) PANIC("Not enough arguments");

        if (strcmp(*arg, "--replacement") == 0) {
            if (sscanf(*++arg, "%" SCNu8, &args.repl_policy) != 1) {
                PANIC("Not an integer: %s", *arg);
            }
            if (args.repl_policy > 2) {
                PANIC("Not a valid replacement policy argument: %s", *arg);
            }
        } else if (strcmp(*arg, "--asm") == 0) {
            args.asm_path = *++arg;
        } else if (strcmp(*arg, "--bin") == 0) {
            args.bin_path = *++arg;
        } else {
            PANIC("Invalid argument: %s", *arg);
        }
    }

    if (!args.asm_path) {
        PANIC("--asm path should be set.")
    }

    if (!args.bin_path && args.repl_policy == UINT8_MAX) {
        PANIC("Either --bin or --replacement should be set.")
    }

    return args;
}

int main(int argc, char* argv[]) {
    struct args args = parse_args(argc, argv);

    FILE* asm_fp;
    asm_fp = fopen(args.asm_path, "r");
    if (!asm_fp) PANIC("Failed to open file %s", args.asm_path);

    FILE* bin_fp = NULL;
    if (args.bin_path) {
        bin_fp = fopen(args.bin_path, "wb");
        if (!bin_fp) PANIC("Failed to open file %s", args.bin_path);
    }

    char line_buf[256];
    struct intermediate instructions[16 * 1024];
    uint32_t i = 0;

    while (fgets(line_buf, sizeof line_buf, asm_fp)) {
        line_buf[strcspn(line_buf, "\n")] = '\0';

        struct intermediate ir = parse_skkv_asm(line_buf);
        instructions[i++] = ir;

        if (bin_fp) {
            uint32_t bin = ir_to_bin(ir);
            fwrite(&bin, 4, 1, bin_fp);
        }
    }

    if (args.repl_policy == 0 || args.repl_policy == 1) {
        struct machine m = machine_new(CACHE_LRU);
        while ((m.pc / 4) < i) {
            uint32_t pc = m.pc / 4;
            m.pc += 4;
            machine_process_ir(&m, instructions[pc]);
        }
        printf("LRU\thit rate: %3.4f%%\n", 100 * (float)m.mem.hits / (float)m.mem.total);
    }

    if (args.repl_policy == 0 || args.repl_policy == 2) {
        struct machine m = machine_new(CACHE_BIT_PLRU);
        while ((m.pc / 4) < i) {
            uint32_t pc = m.pc / 4;
            m.pc += 4;
            machine_process_ir(&m, instructions[pc]);
        }
    
        printf("pLRU\thit rate: %3.4f%%\n", 100 * (float)m.mem.hits / (float)m.mem.total);
    }

    fclose(asm_fp);
    if (bin_fp) fclose(bin_fp);

    return 0;
}
