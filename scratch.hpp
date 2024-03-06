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
    static std::vector<int> reads(const std::string &str);
    static std::string shows(const std::vector<int> &nums);
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
        std::string err_msg;
    public:
        Exception(const char *err_msg);
        ~Exception() = default;
        Exception(const Exception &other) = default;
        const char *what(void) const throw ();
    };
    typedef int Cell;
    static constexpr Cell BLACK = 1;
    static constexpr Cell WHITE = 0;
    class Generator1D {
        void (*callback)(const Cell *line, std::size_t line_sz);
        Cell *line;
        std::size_t line_sz;
        int *info;
        std::size_t info_sz;
    public:
        Generator1D();
        ~Generator1D() = default;
        Generator1D(const Generator1D &other) = default;
        void exec(void);
        bool attach(void (*callback)(const Cell *line, std::size_t line_sz));
        void print(void) const;
        bool init(Cell *line, std::size_t line_sz, int *info, std::size_t info_sz);
    private:
        int run(Cell *rel_coord, int depth, int block_num);
    };
    class Board {
        std::vector<std::vector<Cell>> board;
    public:
        Board(std::vector<std::vector<Cell>> board);
        void print(void) const;
    };
    class Solver {
        const std::vector<std::vector<int>> rows;
        const std::vector<std::vector<int>> cols;
        const std::size_t m;
        const std::size_t n;
        Cell *board;
        std::vector<Board> solutions;
    public:
        Solver(const std::vector<std::vector<int>> &rows, const std::vector<std::vector<int>> &cols);
        Solver() = delete;
        ~Solver();
        Solver(const Solver &other) = default;
        const std::vector<Board> &solve(void);
        void clear(void);
    private:
        bool isAnswer(void);
        int run(Cell *start_point, int depth, int i, int block_num);
        Cell &at(int i, int j);
        const Cell &at(int i, int j) const;
        void print(void) const;
        std::vector<std::vector<Cell>> toMatrix(void) const;
    };
    class SolverV2 {
    public:
        typedef enum Value : int { Error = 'E', Unknown = '?', Sharp = '#', Empty = '.', Contradiction = 'x' } Value_t;
    private:
        const std::vector<std::vector<int>> rows;
        const std::vector<std::vector<int>> cols;
        const std::size_t m;
        const std::size_t n;
        std::vector<bool> rows_done;
        std::vector<bool> cols_done;
        Value_t *board;
    public:
        SolverV2(const std::vector<std::vector<int>> &rows, const std::vector<std::vector<int>> &cols);
        SolverV2() = delete;
        ~SolverV2();
        std::string solve(void);
    private:
        Value_t &at(const std::size_t i, const std::size_t j)
        {
            return board[i * n + j];
        }
        const Value_t &at(const std::size_t i, const std::size_t j) const
        {
            return board[i * n + j];
        }
        bool checkRow(std::size_t i);
        bool checkCol(std::size_t j);
        static std::vector<Value_t> solveLine(const std::vector<Value_t> &line, const std::vector<int> &info);
        class Generator {
            Cell *line;
            const std::size_t line_sz;
            const std::vector<int> &info;
            const std::size_t info_sz;
        public:
            Generator(std::size_t line_sz, const std::vector<int> &info);
            Generator(const Generator &other) = delete;
            ~Generator();
            std::vector<std::vector<Cell>> findAllPossiblitiesCompatibleWith(const std::vector<Value_t> &reference);
        private:
            Generator() = delete;
            bool accumulatePossiblities(Cell *start_point, std::size_t block_num, const std::vector<Value_t> &line_ref, std::vector<std::vector<Cell>> &accum);
        };
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
    SolverV2 mkSolverV2(void) const;
    static Nonogram scanPuzzle(const char *file_name);
};

class SerialPrinter {
    typedef unsigned char byte;
    char const *const msg_prefix;
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

namespace gblobj
{
    extern SerialPrinter sout;
    extern SerialPrinter serr;
}

namespace prog
{
    void solvenonogram(void);
    void scratch(void);
}

namespace test
{
    void io(void);
    void nonogramsolver(void);
    void nonogramsolverv2(void);
    void nonogramsolverlogic(void);
    void serialprinter(void);
    void foo(void);
}

#endif
