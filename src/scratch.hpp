#ifndef SCRATCH_HPP
#define SCRATCH_HPP

#include <chrono>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include "cscratch.h"

class IntegerHelper {
public:
    static std::vector<int> scanIntegers(const std::string &str);
    static std::string showIntegers(const std::vector<int> &nums);
};

class IO {
    char buffer[64];
    int cursor;
    int theend;
    char *result;
    void (*prompt)(const char *msg);
public:
    IO() = default;
    IO(const IO &other) = delete;
    ~IO() = default;
    void setPrompt(void (*prompt)(const char *msg));
    bool runPrompt(void);
    static int getc(void);
private:
    bool takech(int ch);
    void print(void) const;
    void clear(void);
    void sync(char *&msg) const;
};

class Nonogram {
public:
    class Exception : public std::exception {
    private:
        std::string err_msg;
    public:
        Exception(const char *err_msg);
        ~Exception() = default;
        Exception(const Exception &other) = default;
        const char *what(void) const throw ();
    };
    class Solver {
    public:
        typedef int Cell;
        static constexpr Cell BOTHPOSSIBLE = 0;
        static constexpr Cell WHITE = 1;
        static constexpr Cell BLACK = 2;
        static constexpr Cell INDETERMINED = 3;
    private:
        const std::size_t m;
        const std::size_t n;
        const std::vector<std::vector<int>> rows;
        const std::vector<std::vector<int>> cols;
        std::vector<bool> row_dones;
        std::vector<bool> col_dones;
        Cell *board;
    public:
        Solver(const std::vector<std::vector<int>> &rows, const std::vector<std::vector<int>> &cols);
        Solver() = delete;
        ~Solver();
        Solver(const Solver &rhs) = default;
        std::string solve(void);
        void clear(void);
    private:
        Cell &at(std::size_t i, std::size_t j);
        bool traverseRow(std::size_t i, const std::vector<int> &row);
        bool traverseCol(std::size_t j, const std::vector<int> &col);
    };
private:
    std::vector<std::vector<int>> rows;
    std::vector<std::vector<int>> cols;
public:
    Nonogram(const std::vector<std::vector<int>> &rows, const std::vector<std::vector<int>> &cols);
    Nonogram() = delete;
    ~Nonogram() = default;
    Nonogram(const Nonogram &other) = default;
    bool isWellFormed(void) const;
    Solver mkSolver(void) const;
    static Nonogram scanPuzzle(const char *file_name);
};

class SerialPrinter {
    typedef unsigned char byte;
    const char *const msg_prefix;
    bool mknewline;
public:
    SerialPrinter() = delete;
    SerialPrinter(const SerialPrinter &other) = delete;
    ~SerialPrinter();
    SerialPrinter(SerialPrinter &&other);
    SerialPrinter(const char *prefix);
    SerialPrinter operator<<(bool b);
    SerialPrinter operator<<(byte x);
    SerialPrinter operator<<(int n);
    SerialPrinter operator<<(const char *s);
    SerialPrinter operator<<(char c);
    SerialPrinter operator<<(double v);
    SerialPrinter operator<<(const std::string &s);
    SerialPrinter operator<<(const std::stringstream &ss);
private:
    SerialPrinter(const char *prefix, bool lend);
    void trick();
};

namespace scratch
{
    extern SerialPrinter sout;
    extern SerialPrinter serr;
    void solvenonogram(void);
    void main(void);
}

namespace test
{
    void io(void);
    void serialprinter(void);
    void foo(void);
}

#endif
