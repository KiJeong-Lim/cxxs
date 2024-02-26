#ifndef CSCRATCH_H
#define CSCRATCH_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define len(xs)     (sizeof(xs)/sizeof((xs)[0]))

struct OS_C_API {
    void (*delay)(unsigned long int sleeping_time);
    int (*kbhit)(void);
    int (*getch)(void);
    long long int (*itime)(void);
    void (*clear)(void);
};

size_t int2size_t(int n);

#endif
