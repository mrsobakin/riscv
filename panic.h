#ifndef _PANIC_H
#define _PANIC_H

#include <stdio.h>
#include <stdlib.h>

#define PANIC(fmt, ...) { fprintf(stderr, "[%s:%i] \e[0;31mPanic:\e[0m " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); exit(1); }

#endif // _PANIC_H
