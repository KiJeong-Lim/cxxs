#ifndef SCRATCH_HPP
#define SCRATCH_HPP

#include <chrono>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

#include "cscratch.h"

extern "C" {
    extern const struct OS_C_API os;
};

class IntegerHelper {
public:
    static std::vector<int> reads(const std::string &str);
    static void shows(const std::vector<int> &nums);
};

class IO {
    char buffer[64];
    int cursor;
    int theend;
    char *result;
    void (*prompt)(const char *msg);
public:
    IO() = default;
    IO(const IO &other) = default;
    ~IO() = default;
    void setPrompt(void (*prompt)(const char *msg));
    bool runPrompt(void);
    static int getc(void);
private:
    bool takech(int ch);
    void print(void);
    void clear(void);
    void sync(char *&msg);
};

void test_io(void);
void test_nonogramsolver(void);
void test_nonogramsolverv2(void);
void test_nonogramsolverlogic(void);

#endif
