#ifndef _PARSER_H
#define _PARSER_H

#include "core.h"

uint8_t parse_rs(const char* str);
int32_t parse_num(const char* str);
struct intermediate parse_skkv_asm(const char* line);

#endif // _PARSER_H
