#include "scratch.hpp"

#define ESC 	        27
#define LEFT_DIRECTION  75
#define RIGHT_DIRECTION 77

static void prompt(const char *msg);

void IO::setPrompt(void (*const prompt)(const char *msg))
{
    this->prompt = prompt;
    this->clear();
}

bool IO::runPrompt()
{
    char *msg = NULL;
    int ch = '\0';
    bool prompt_routine_breaked = false;

    while (1) {
        ch = getc();
        if (ch == 0) {
            return false;
        }
        if (ch == ESC) {
            return true;
        }
        prompt_routine_breaked = takech(ch);
        if (prompt_routine_breaked) {
            sync(msg);
            prompt(msg);
            clear();
            return true;
        }
    }
}

int IO::getc()
{
    int res = 0;

    if (os.kbhit()) {
        res = os.getch();
    }

    return res;
}

void IO::clear()
{
    for (int i = 0; i < len(buffer); i++) {
        buffer[i] = '\0';
    }
    cursor = 0;
    theend = 0;
}

bool IO::takech(const int ch)
{
    switch (ch) {
    default:
        if (cursor < 0) {
            result = NULL;
            return false;
        }
        if (cursor > theend) {
            result = NULL;
            return false;
        }
        if (theend + 1 >= len(buffer)) {
            result = NULL;
            return false;
        }
        for (int i = theend; i >= cursor; i--) {
            buffer[i + 1] = buffer[i];
        }
        buffer[cursor++] = ch;
        buffer[++theend] = '\0';
        print();
        result = NULL;
        return false;
    case '\b':
        if (cursor > theend) {
            result = NULL;
            return false;
        }
        if (theend + 1 >= len(buffer)) {
            result = NULL;
            return false;
        }
        if (cursor <= 0) {
            result = NULL;
            return false;
        }
        for (int i = --cursor; i < theend; i++) {
            buffer[i] = buffer[i + 1];
        }
        if (theend > 0) {
            buffer[theend--] = '\0';
        }
        print();
        result = NULL;
        return false;
    case '\n':
    case '\r':
        result = buffer;
        return true;
    case '\0':
        return false;
    case ESC:
        clear();
        printf("\n");
        result = NULL;
        return true;
    case 224:
        if (theend + 1 >= len(buffer)) {
            result = NULL;
            return false;
        }
        switch (getc()) {
        case LEFT_DIRECTION:
            if (cursor > 0) {
                cursor--;
            }
            print();
            result = NULL;
            return false;
        case RIGHT_DIRECTION:
            if (cursor < theend) {
                cursor++;
            }
            print();
            result = NULL;
            return false;
        }
    }
    return false;
}

void IO::sync(char *&msg)
{
    msg = result;
}

void IO::print()
{
    int i = 0;

    printf("\r");
    for (i = 0; i < len(buffer); i++) {
        printf(" ");    
    }
    printf("\r");
    for (i = 0; i < cursor; i++) {
        printf(" ");
    }
    for (i = cursor; i < theend; i++) {
        printf("%c", buffer[i]);
    }
    buffer[i] = '\0';
    printf("\r");
    for (int i = 0; i < cursor; i++) {
        printf("%c", buffer[i]);
    }
    std::cout.flush();
}

void prompt(const char *const msg)
{
    printf("\n[ECHO] %s\n", msg);
}

void test_io()
{
    IO comm;
    bool prompt_finished = false;

    comm.setPrompt(prompt);

    while(!prompt_finished) {
        os.sleep(10);
        prompt_finished = comm.runPrompt();
    }
}
