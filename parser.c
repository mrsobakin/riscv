#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>

#include "core.h"
#include "instructions.h"
#include "panic.h"
#include "parser.h"

uint8_t parse_rs(const char* str) {
    char c;
    uint8_t i;

    if(sscanf(str, "%c%" SCNu8, &c, &i) == 2) {
        if (c == 'x' && i <= 31) {
            return i;
        }
        if (c == 't' && i <= 6) {
            if (i < 3) {
                return i + 5;
            } else {
                return i + 28 - 3;
            }
        }
        if (c == 's' && i <= 11) {
            if (i < 2) {
                return i + 8;
            } else {
                return i + 18 - 2;
            }
        }
        if (c == 'a' && i <= 7) {
            return i + 10;
        }
    }

    if (strcmp(str, "zero") == 0) return 0;
    if (strcmp(str, "ra"  ) == 0) return 1;
    if (strcmp(str, "sp"  ) == 0) return 2;
    if (strcmp(str, "gp"  ) == 0) return 3;
    if (strcmp(str, "tp"  ) == 0) return 4;
    if (strcmp(str, "fp"  ) == 0) return 8;

    PANIC("Not a registry: \"%s\"", str);
}

uint8_t parse_iorw(const char* str) {
    uint8_t iorw = 0b0000;
    uint8_t mask;

    for (const char* c = str; *c != '\0'; ++c) {
        switch (*c) {
            case 'i':
                mask = 0b1000;
                break;
            case 'o':
                mask = 0b0100;
                break;
            case 'r':
                mask = 0b0010;
                break;
            case 'w':
                mask = 0b0001;
                break;
            default:
                PANIC("Invalid flag");
        }

        if (iorw & mask) {
            PANIC("Flag already used");
        }

        iorw |= mask;
    }

    return iorw;
}

int32_t parse_num(const char* str) {
    int32_t num;

    if (!(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))) {
        if(sscanf(str, "%" SCNd32, &num) == 1) {
            return num;
        }
        PANIC("Not a number: %s", str);
    }

    bool neg = false;
    if (str[0] == '-') {
        neg = true;
        str++;
    }

    if(sscanf(str, "0%*1[xX]%x" SCNd32, &num) == 1) {
        return neg ? -num : num;
    }

    PANIC("Not a number: %s", str);
}

struct intermediate parse_skkv_asm(const char* line) {
    int n_parsed;
    char s_instr[16], s_a[16], s_b[16], s_c[16];
    enum instr instr;
    int32_t a = 0, b = 0, c = 0;

    n_parsed = sscanf(line, " %[^, ] %[^, ] , %[^,( ] %*[,(] %[^,) ] ", s_instr, s_a, s_b, s_c);

#ifdef DEBUG
    printf("instr:\t\"%s\"\n", s_instr);
    printf("a:\t\"%s\"\n", s_a);
    printf("b:\t\"%s\"\n", s_b);
    printf("c:\t\"%s\"\n\n", s_c);
#endif

    if (n_parsed < 1)
        PANIC("Empty line");

    for(char *p=s_instr; *p; p++)
        *p=tolower(*p);

    instr = parse_instr(s_instr);

    if (n_parsed < instr_n_args(instr))
        PANIC("Not enough arguments");

    const char* t = instr_args_types(instr);

    if (t[0] == 'r')
        a = parse_rs(s_a);
    else if (t[0] == 'n')
        a = parse_num(s_a);

    if (t[1] == 'r')
        b = parse_rs(s_b);
    else if (t[1] == 'n')
        b = parse_num(s_b);

    if (t[2] == 'r')
        c = parse_rs(s_c);
    else if (t[2] == 'n')
        c = parse_num(s_c);

    if (instr == INSTR_FENCE) {
        a = parse_iorw(s_a);
        b = parse_iorw(s_b);
    }

#ifdef DEBUG
    printf("instr:\t%i\n", instr);
    printf("a:\t%i\n", a);
    printf("b:\t%i\n", b);
    printf("c:\t%i\n\n", c);
#endif

    return (struct intermediate) {
        .instr = instr,
        .a = a,
        .b = b,
        .c = c,
    };
}
