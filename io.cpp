#include "scratch.hpp"

#define ESC             27
#define LEFT_DIRECTION  75
#define RIGHT_DIRECTION 77
#define DEL_KEY         83

static void test_prompt(const char *msg);
static char hex2char(unsigned int hex);

SerialPrinter gblobj::sout{ .prefix = "arduino> " };
SerialPrinter gblobj::serr{ .prefix = "WARNING> " };

void test::io()
{
    IO comm{ };
    bool prompt_finished = false;

    comm.setPrompt(test_prompt);

    while(!prompt_finished) {
        prompt_finished = comm.runPrompt();
        os.delay(5);
    }
}

void test::serialprinter()
{
    gblobj::sout << "hello" << ' ' << 1234 << ' ' << 3.141592;
    gblobj::sout << "world" << ' ' << 5678 << ' ' << 2.718281;
}

void test_prompt(const char *const msg)
{
    std::cout << "\n[ECHO] " << msg << std::endl;
}

char hex2char(const unsigned int hex)
{
    if (hex >= 16)
        return '\0';
    else
        return "0123456789ABCDEF"[hex];
}

void IO::setPrompt(void (*const prompt)(const char *msg))
{
    this->prompt = prompt;
    this->clear();
}

bool IO::runPrompt()
{
    char *msg = nullptr;
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
    for (std::size_t i = 0; i < len(buffer); i++) {
        buffer[i] = '\0';
    }
    cursor = 0;
    theend = 0;
}

bool IO::takech(const int ch)
{
    switch (special_key_flag) {
    case 0x00:
        result = nullptr;
        return false;
    case 0xE0:
        switch (ch) {
        case DEL_KEY:
            if (theend > cursor)
                buffer[--theend] = '\0';
            print();
            result = nullptr;
            return false;
        case LEFT_DIRECTION:
            if (cursor > 0) {
                cursor--;
            }
            print();
            result = nullptr;
            return false;
        case RIGHT_DIRECTION:
            if (int2size_t(theend) >= len(buffer)) {
                result = nullptr;
                return false;
            }
            if (cursor < theend) {
                cursor++;
            }
            print();
            result = nullptr;
            return false;
        }
    case -1:
        switch (ch) {
        default:
            if (cursor < 0) {
                result = nullptr;
                return false;
            }
            if (cursor > theend) {
                result = nullptr;
                return false;
            }
            if (int2size_t(theend) + 1 >= len(buffer)) {
                result = nullptr;
                return false;
            }
            for (int i = theend; i >= cursor; i--) {
                buffer[i + 1] = buffer[i];
            }
            buffer[cursor++] = ch;
            buffer[++theend] = '\0';
            print();
            result = nullptr;
            return false;
        case '\b':
            if (cursor > theend) {
                result = nullptr;
                return false;
            }
            if (int2size_t(theend) >= len(buffer)) {
                if (cursor > 0)
                    cursor--;
                buffer[theend--] = '\0';
                result = nullptr;
                return false;
            }
            if (cursor <= 0) {
                result = nullptr;
                return false;
            }
            cursor--;
            for (int i = cursor; i < theend; i++) {
                buffer[i] = buffer[i + 1];
            }
            if (theend > 0) {
                buffer[theend--] = '\0';
            }
            print();
            result = nullptr;
            return false;
        case '\n':
        case '\r':
            result = buffer;
            return true;
        case '\0':
            return false;
        case ESC:
            clear();
            std::cout << '\n';
            result = nullptr;
            return true;
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
    std::cout << '\r';
    for (i = 0; int2size_t(i) < len(buffer); i++)
        std::cout << ' ';
    std::cout << '\r';
    for (i = 0; i < cursor; i++)
        std::cout << ' ';
    for (i = cursor; i < theend; i++)
        std::cout << buffer[i];
    buffer[i] = '\0';
    std::cout << '\r';
    for (i = 0; i < cursor; i++)
        std::cout << buffer[i];
    std::cout.flush();
}

SerialPrinter::SerialPrinter(SerialPrinter &&other)
    : msg_prefix{ other.msg_prefix }, mknewline{ other.mknewline }
{
    other.mknewline = false;
}

SerialPrinter::SerialPrinter(const char *const prefix)
    : msg_prefix{ prefix }, mknewline{ false }
{
}

SerialPrinter::SerialPrinter(const char *const prefix, const bool lend)
    : msg_prefix{ prefix }, mknewline{ lend }
{
}

SerialPrinter::~SerialPrinter()
{
    if (mknewline)
        std::cout << '\n';
    os.delay(5);
}

void SerialPrinter::trick()
{
    mknewline = false;
    if (msg_prefix != nullptr)
        std::cout << msg_prefix;
}

SerialPrinter SerialPrinter::operator<<(const bool b)
{
    trick();
    std::cout << (b ? "true" : "false");
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const SerialPrinter::byte x)
{
    trick();
    std::cout << "0x" << hex2char(x / 16) << hex2char(x % 16);
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const int n)
{
    trick();
    std::cout << n;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const char *const s)
{
    trick();
    std::cout << s;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const double v)
{
    trick();
    std::cout << v;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const std::string &s)
{
    trick();
    std::cout << s;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const std::stringstream &ss)
{
    trick();
    std::cout << ss.str();
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const char c)
{
    trick();
    std::cout << c;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}
