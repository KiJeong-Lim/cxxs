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
        void (*callback)(Cell *line, std::size_t line_sz);
        Cell *line;
        std::size_t line_sz;
        int *info;
        std::size_t info_sz;
    public:
        Generator1D();
        ~Generator1D() = default;
        Generator1D(const Generator1D &other) = default;
        void exec(void);
        bool attach(void (*callback)(Cell *line, std::size_t line_sz));
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
        Solver(std::vector<std::vector<int>> rows, std::vector<std::vector<int>> cols);
        Solver() = delete;
        ~Solver();
        Solver(const Solver &other) = default;
        std::vector<Board> solve(void);
        void clear(void);
    private:
        bool isAnswer(void);
        int run(Cell *start_point, int depth, int i, int block_num);
        Cell &at(int i, int j);
        const Cell &at(int i, int j) const;
        void print(void) const;
        std::vector<std::vector<Cell>> toMatrix(void) const;
    };
private:
    std::vector<std::vector<int>> rows;
    std::vector<std::vector<int>> cols;
public:
    Nonogram(std::vector<std::vector<int>> rows, std::vector<std::vector<int>> cols);
    Nonogram() = delete;
    ~Nonogram() = default;
    Nonogram(const Nonogram &other) = default;
    static Nonogram scan(const char *file_name);
    bool isWellFormed(void) const;
    Solver mksolver(void) const;
};

void test_io(void);
void test_nonogramsolver(void);
void test_nonogramsolverlogic(void);

#endif
