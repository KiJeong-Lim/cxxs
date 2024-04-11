#ifndef SCRATCH_HPP
#define SCRATCH_HPP

#include <chrono>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "cscratch.h"

template <typename ELEM>
using Array = std::vector<ELEM>;

using String = std::string;

using Nat = std::size_t;

template <typename ARRAY>
constexpr Nat len(const ARRAY &xs)
{
    return xs.size();
}

template <typename ELEM, Nat SIZE>
constexpr Nat len(const ELEM (&xs)[SIZE]) noexcept
{
    return SIZE;
}

class IntegerHelper {
public:
    static Array<int> scanIntegers(const String &str);
    static String showIntegers(const Array<int> &nums);
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
        String err_msg;
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
        const Array<Array<int>> rows;
        const Array<Array<int>> cols;
        Array<bool> row_dones;
        Array<bool> col_dones;
        Cell *board;
    public:
        Solver(const Array<Array<int>> &rows, const Array<Array<int>> &cols);
        Solver() = delete;
        ~Solver();
        Solver(const Solver &rhs) = default;
        String solve(void);
        void clear(void);
    private:
        Cell &at(std::size_t i, std::size_t j);
        bool traverseRow(std::size_t i);
        bool traverseCol(std::size_t j);
    };
private:
    Array<Array<int>> rows;
    Array<Array<int>> cols;
public:
    Nonogram(const Array<Array<int>> &rows, const Array<Array<int>> &cols);
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
    SerialPrinter operator<<(const String &s);
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
    void fifteenpuzzle(void);
    void main(void);
}

namespace test
{
    void io(void);
    void heap(void);
    void serialprinter(void);
    void foo(void);
}

#endif
