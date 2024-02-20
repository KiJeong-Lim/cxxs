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
    static std::vector<int> reads(std::string str);
    static void shows(std::vector<int> nums);
};

class IO {
    char buffer[64];
    int  cursor;
    int  theend;
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

namespace Nonogram {

    typedef int Cell;

    class Generator1D {
    public:
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
        int run(Cell *rel_coord, int depth, int block_num, int combo);
    };

    static constexpr Cell BLACK = 1;

    static constexpr Cell WHITE = 0;

    class Exception : public std::exception {
        std::string err_msg;
    public:
        Exception(const char *err_msg);
        ~Exception() = default;
        Exception(const Exception &other) = default;
        const char *what(void) const throw ();
    };

    class Answer {
        std::vector<std::vector<Cell>> board;
    public:
        Answer(std::vector<std::vector<Cell>> board);
        ~Answer() = default;
        Answer(const Answer &other) = default;
        void print(void) const;
    };

    class Solver {
        std::vector<std::vector<int>> rows;
        std::vector<std::vector<int>> cols;
        Cell *board;
        int m;
        int n;
        std::vector<Answer> solutions;
    public:
        Solver() = default;
        ~Solver();
        Solver(const Solver &other) = default;
        bool setPuzzle(const std::vector<std::vector<int>> &rows, const std::vector<std::vector<int>> &cols);
        bool puzzleWellFormed(void) const;
        std::vector<std::vector<Cell>> toMatrix(void) const;
        bool scanPuzzle(const char *file_name);
        void clear(void);
        void solve(void);
        const std::vector<Answer> &getSolutions(void) const;
    private:
        bool isAnswer(void);
        int run(Cell *start_point, int depth, int i, int block_num);
        Cell &at(int i, int j);
        const Cell &at(int i, int j) const;
        void print(void) const;
    };
}

void test_io(void);
void test_nonogramsolver(void);
void test_nonogramsolverlogic(void);

#endif
