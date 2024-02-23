#ifndef CSCRATCH_H
#define CSCRATCH_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define len(xs)         (sizeof(xs)/sizeof((xs)[0]))

struct OS_C_API {
    void (*sleep)(long unsigned int sleeping_time);
    int (*kbhit)(void);
    int (*getch)(void);
    long long int (*itime)(void);
    void (*clear)(void);
};

#endif
