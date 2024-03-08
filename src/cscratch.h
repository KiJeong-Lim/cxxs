#ifndef CSCRATCH_H
#define CSCRATCH_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NOT_A_SPECIAL_KEY   (-1)
#define len(xs)             (sizeof(xs)/sizeof((xs)[0]))

struct OS_C_API {
    void (*delay)(unsigned long int sleeping_time);
    int (*kbhit)(void);
    int (*getch)(void);
    long long int (*itime)(void);
    void (*clear)(void);
};

size_t int2size_t(int n);
void print_and_wait(const char *msg);
void press_any_key(void);

extern const struct OS_C_API os;
extern int special_key_flag;

#endif
