#include "scratch.hpp"

#define DEBUG 0

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
    class SolverV2 {
    public:
        typedef enum Value : int { Error = -1, Unknown = 0, Sharp = 1, Empty = 2, Contradiction = 3 } Value_t;
    private:
        const std::vector<std::vector<int>> rows;
        const std::vector<std::vector<int>> cols;
        const std::size_t m;
        const std::size_t n;
        std::vector<bool> rows_done;
        std::vector<bool> cols_done;
        bool solved;
        Value_t *board;
    public:
        SolverV2(const std::vector<std::vector<int>> &rows, const std::vector<std::vector<int>> &cols);
        ~SolverV2();
        bool solve(void);
        Value_t &at(int i, int j);
        const Value_t &at(int i, int j) const;
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

template <typename ELEM> using Array = std::vector<ELEM>;

static void debug_Generator1D_callback(const Nonogram::Cell *line, std::size_t sz);
static long long int generator1d_callback_call_count = 0;

void test_nonogramsolver()
{
    try {
        Nonogram puzzle = Nonogram::scanPuzzle("nonogramtest.txt");
        Nonogram::Solver solver = puzzle.mkSolver();
        Array<Nonogram::Board> solutions = solver.solve();
        for (auto sol = solutions.cbegin(); sol != solutions.cend(); ++sol)
            sol->print();
    }
    catch (const std::exception &e) {
        std::cerr << e.what();
    }
}

void test_nonogramsolverv2()
{
    try {
        Nonogram puzzle = Nonogram::scanPuzzle("nonogramtest.txt");
        Nonogram::SolverV2 solver = puzzle.mkSolverV2();
        solver.solve();
    }
    catch (const std::exception &e) {
        std::cerr << e.what();
    }
}

bool Nonogram::SolverV2::solve()
{
    // to do

    return solved;
}

void debug_Generator1D_callback(const Nonogram::Cell *const line, const std::size_t line_sz)
{
    std::cout << "================\n";
    std::cout << "0123456789ABCDEF\n";
    for (std::size_t i = 0; i < line_sz; i++) {
        switch (line[i]) {
        case Nonogram::BLACK:
            std::cout << '#';
            break;
        case Nonogram::WHITE:
            std::cout << '.';
            break;
        default:
            std::cout << 'E';
            break;
        }
    }
    std::cout << std::endl;
    std::cout << "================\n";
    std::cout << std::endl;

    generator1d_callback_call_count++;
}

void test_nonogramsolverlogic()
{
    int info[] = { 2, 2, 3, 1, };
    Nonogram::Cell line[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
    Nonogram::Generator1D gen = {};
    bool well_formed = gen.init(line, len(line), info, len(info));
    
    if (well_formed) {
        gen.attach(debug_Generator1D_callback);
        gen.exec();
        std::cout << "count = " << generator1d_callback_call_count << std::endl;
    }
    else
        std::cout << "***test_nonogramsolverlogic(): ill-formed\n";
}

Nonogram::SolverV2::SolverV2(const std::vector<std::vector<int>> &rows, const std::vector<std::vector<int>> &cols)
    : rows{ rows }, cols{ cols }, m{ rows.size() }, n{ cols.size() }, rows_done{ }, cols_done{ }, solved{ false }, board{ nullptr }
{
}

Nonogram::SolverV2::~SolverV2()
{
    delete board;
    board = nullptr;
}

Nonogram::SolverV2::Value_t &Nonogram::SolverV2::at(const int i, const int j)
{
    return board[i * n + j];
}

const Nonogram::SolverV2::Value_t &Nonogram::SolverV2::at(const int i, const int j) const
{
    return board[i * n + j];
}

Nonogram::Generator1D::Generator1D()
    : callback{ nullptr }, line{ nullptr }, line_sz{ 0 }
{
}

void Nonogram::Generator1D::exec()
{
    int block_cnt = 0;
    bool last_was_black = false;

    for (int i = 0; i < line_sz; i++)
        line[i] = WHITE;

    run(line, info_sz, 0);
}

bool Nonogram::Generator1D::attach(void (*const callback)(const Nonogram::Cell *line, size_t line_sz))
{
    if (callback == nullptr) {
        return false;
    }

    this->callback = callback;
    return true;
}

void Nonogram::Generator1D::print() const
{
    for (int i = 0; i < line_sz; i++) {
        switch (line[i]) {
        case BLACK:
            std::cout << '#';
            break;
        case WHITE:
            std::cout << '.';
            break;
        default:
            std::cout << 'E';
            break;
        }
    }
    std::cout << std::endl;
}

bool Nonogram::Generator1D::init(Nonogram::Cell *const line, const std::size_t line_sz, int *const info, const std::size_t info_sz)
{
    this->line_sz = line_sz;
    this->line = line;
    this->info = info;
    this->info_sz = info_sz;

    return true;
}

int Nonogram::Generator1D::run(Nonogram::Cell *const start, const int depth, const int block_num)
{
#if defined(DEBUG)
#if DEBUG > 0
    char char128[128];
    printf("[CALLED: depth = %d, start = %d, block_num = %d] ", depth, start - line, block_num);
    print();
    printf("\n");
    gets(char128);
#endif
#endif

    if (depth == 0) {
        callback(line, line_sz);
        return 1;
    }
    else {
        const int block_sz = info[block_num];
        Cell *left = start, *right = start;
        int t = 0, res = 0, s = 0;

        if (left + block_sz > line + line_sz) {
            return 0;
        }
        
        for (int b = 0; b < block_sz; b++)
            *right++ = BLACK;

        while (run(right + 1, depth - 1, block_num + 1)) {
            if (right >= line + line_sz)
                break;
            *left++ = WHITE;
            *right++ = BLACK;
        }

        while (right > left)
            *--right = WHITE;

        return 1;
    }
}

Nonogram::Exception::Exception(const char *const err_msg)
    : err_msg{ err_msg }
{
}

const char *Nonogram::Exception::what() const throw ()
{
    return err_msg.c_str();
}

Nonogram::Solver::~Solver()
{
    clear();
}

Nonogram::Board::Board(const Array<Array<Nonogram::Cell>> board)
    : board{ board }
{
}

void Nonogram::Board::print() const
{
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            switch (board[i][j]) {
            case BLACK:
                std::cout << '#';
                break;
            case WHITE:
                std::cout << '.';
                break;
            default:
                std::cout << 'E';
                break;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

Nonogram::Nonogram(const Array<Array<int>> &rows, const Array<Array<int>> &cols)
    : rows{ rows }, cols{ cols }
{    
}

Nonogram Nonogram::scanPuzzle(const char *const file_name)
{
    Array<Array<int>> rows, cols;
    std::ifstream file;
    std::string line;

    try {
        file.open(file_name, std::ios::in);
        if (file.bad()) {
            throw Exception("***Nonogram::scan(): cannot open the file\n");
        }
        getline(file, line);
        if (line != std::string{ "rows" }) {
            throw Exception("***Nonogram::scan(): expected \'rows\'\n");
        }
        while (getline(file, line)) {
            if (line == std::string{ "cols" })
                break;
            rows.push_back(IntegerHelper::reads(line));
        }
        while (getline(file, line)) {
            if (line == std::string{ "end" })
                break;
            cols.push_back(IntegerHelper::reads(line));
        }
        file.close();
    }
    catch (const std::exception &e) {
        std::cerr << e.what();
        file.close();
    }

    return Nonogram(rows, cols);
}

bool Nonogram::isWellFormed() const
{
    const std::size_t m = rows.size(), n = cols.size();

    if (m == 0 || n == 0)
        return false;
    for (int i = 0; i < m; i++)
        if (rows[i].size() == 0)
            return false;
    for (int j = 0; j < n; j++)
        if (cols[j].size() == 0)
            return false;
    for (int i = 0; i < m; i++)
        if (rows[i].size() == 1 && rows[i][0] == 0)
            continue;
        else {
            int t = 0;
            for (int k = 0; k < rows[i].size(); k++)
                if (rows[i][k] <= 0)
                    return false;
                else {
                    if (t == 0)
                        t += rows[i][k];
                    else
                        t += rows[i][k] + 1;
                }
            if (t > m)
                return false;
        }
    for (int j = 0; j < n; j++)
        if (cols[j].size() == 1 && cols[j][0] == 0)
            continue;
        else {
            int t = 0;
            for (int k = 0; k < cols[j].size(); k++)
                if (cols[j][k] <= 0)
                    return false;
                else {
                    if (t == 0)
                        t += cols[j][k];
                    else
                        t += cols[j][k] + 1;
                }
            if (t > n)
                return false;
        }
    return true;
}

Nonogram::Solver Nonogram::mkSolver() const
{
    if (isWellFormed())
        return Solver{ rows, cols };
    else
        throw Exception("***Nonogram::mkSolver(): puzzle not well formed\n");
}

Nonogram::SolverV2 Nonogram::mkSolverV2() const
{
    if (isWellFormed())
        return SolverV2{ rows, cols };
    else
        throw Exception("***Nonogram::mkSolverV2(): puzzle not well formed\n");
}

Nonogram::Solver::Solver(const Array<Array<int>> &rows, const Array<Array<int>> &cols)
    : rows{ rows }, cols{ cols }, m{ rows.size() }, n{ cols.size() }, board{ nullptr }, solutions{ }
{
    if (m > 0 && n > 0)
        board = new Cell [m * n];
    else
        throw Exception("***Nonogram::Solver::Solver(): m == 0 || n == 0\n");
}

Array<Nonogram::Board> Nonogram::Solver::solve()
{
    int num = 0;
    for (int i = 0; i < m; i++)
        num += rows[i].size();
    for (int i = 0; i < m * n; i++)
        board[i] = WHITE;
    run(board, num, 0, 0);
    return solutions;
}

Nonogram::Cell &Nonogram::Solver::at(const int i, const int j)
{
    return board[i * n + j];
}

const Nonogram::Cell &Nonogram::Solver::at(const int i, const int j) const
{
    return board[i * n + j];
}

void Nonogram::Solver::print() const
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            switch (at(i, j)) {
            case BLACK:
                std::cout << '#';
                break;
            case WHITE:
                std::cout << '.';
                break;
            default:
                std::cout << 'E';
                break;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Nonogram::Solver::clear()
{
    delete board;
    board = nullptr;
}

bool Nonogram::Solver::isAnswer()
{
    int black_cnt = 0, k = 0;

    for (int j = 0; j < n; j++) {
        black_cnt = 0;
        k = 0;

        for (int i = 0; i < m; i++)
            if (at(i, j) == BLACK)
                black_cnt++;
            else {
                if (black_cnt > 0) {
                    if (cols[j][k] == black_cnt)
                        k++;
                    else
                        return false;
                }
                black_cnt = 0;
            }
        if (black_cnt > 0) {
            if (cols[j][k] != black_cnt)
                return false;
            else
                k++;
        }
        if (cols[j].size() == 1 && cols[j][0] == 0) {
            if (k != 0)
                return false;
        }
        else {
            if (k != cols[j].size())
                return false;
        } 
    }

    return true;
}

Array<Array<Nonogram::Cell>> Nonogram::Solver::toMatrix() const
{
    Array<Array<Cell>> ret = {};

    for (int i = 0; i < m; i++) {
        Array<Cell> line = {};
        for (int j = 0; j < n; j++)
            line.push_back(at(i, j));
        ret.push_back(static_cast<Array<Cell> &&>(line));
    }
    return ret;
}

int Nonogram::Solver::run(Nonogram::Cell *const start_point, const int depth, const int i, const int block_num)
{
#if defined(DEBUG)
#if DEBUG > 0
    char char128[128];
    printf("[CALLED: start_point = %d, depth = %d, i = %d, block_num = %d]\n", start_point - board, depth, i, block_num);
    print();
    printf("\n");
    gets(char128);
#endif
#endif

    if (depth == 0) {
        if (isAnswer())
            solutions.push_back(Board{toMatrix()});

        return 1;
    }
    else {
        const int block_sz = rows[i][block_num], next_depth = depth - 1;
        Cell *left = start_point, *right = start_point, *next_floor = nullptr;
        int next_i = i, next_block_num = block_num + 1, res = 0;

        if (rows[i].size() == 1 && rows[i][0] == 0)
            return run(&board[(i + 1) * n], next_depth, i + 1, 0);

        if (left + block_sz > &board[(i + 1) * n])
            return 0;

        if (block_num + 1 == rows[i].size()) {
            next_i = i + 1;
            next_floor = &board[(i + 1) * n];
            next_block_num = 0;
        }

        for (int b = 0; b < block_sz; b++)
            *right++ = BLACK;

        if (next_floor == nullptr) {
            while (run(right + 1, next_depth, next_i, next_block_num)) {
                if (right >= &board[(i + 1) * n])
                    break;
                *left++ = WHITE;
                *right++ = BLACK;
            }
        }
        else {
            while (run(next_floor, next_depth, next_i, next_block_num)) {
                if (right >= &board[(i + 1) * n])
                    break;
                *left++ = WHITE;
                *right++ = BLACK;
            }
        }

        while (right > left)
            *--right = WHITE;

        return 1;
    }
}
