#include <time.h>
#include <windows.h>
#include <conio.h>
#include "cscratch.h"

#define SPECIAL_KEY_DEFAULT_FLAG (-1)

static long long int longlongint_time(void);
static void clear_console(void);
static int smart_getch(void);

int special_key_flag = -1;

const struct OS_C_API os = {
    .delay = Sleep,
    .kbhit = _kbhit,
    .getch = smart_getch,
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

int smart_getch()
{
    int ch = _getch();
    switch (ch) {
    case 0x00:
    case 0xE0:
        special_key_flag = ch;
        return getch();
    default:
        special_key_flag = NOT_A_SPECIAL_KEY;
        return ch;
    }
}

void print_and_wait(const char *const msg)
{
    printf("%s\n", msg);
    os.getch();
}

void press_any_key()
{
    print_and_wait("Press any key...");
}

