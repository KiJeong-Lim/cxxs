#include <time.h>
#include <windows.h>
#include <conio.h>
#include "cscratch.h"

static long long int longlongint_time(void);
static void clear_console(void);

struct OS_C_API os = {
    .delay = Sleep,
    .kbhit = kbhit,
    .getch = getch,
    .itime = longlongint_time,
    .clear = clear_console,
};

time_t longlongint_time()
{
    return time(NULL);
}

void clear_console()
{
    system("cls");
}

size_t int2size_t(int n)
{
    if (n > 0) {
        size_t result = 0;
        while (--n >= 0)
            result++;
        return result;
    }
    else
        return 0;
}
