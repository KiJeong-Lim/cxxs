#include "scratch.hpp"

#define DEBUG 0

static void debug_Generator1D_callback(Nonogram::Cell *line, std::size_t sz);
static long long int counter = 0;

void test_nonogramsolver(void);
void test_nonogramsolverlogic(void);

void test_nonogramsolver()
{
    try {
        Nonogram puzzle = Nonogram::scan("nonogramtest.txt");
        Nonogram::Solver solver = puzzle.mksolver();
        Array<Nonogram::Board> solutions = solver.solve();
        for (auto sol = solutions.cbegin(); sol != solutions.cend(); ++sol)
            sol->print();
    }
    catch (const std::exception &e) {
        std::cerr << e.what();
    }
}

void debug_Generator1D_callback(Nonogram::Cell *const line, const size_t line_sz)
{
    std::cout << "================\n";
    std::cout << "0123456789ABCDEF\n";
    for (int i = 0; i < line_sz; i++) {
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

    counter++;
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
        printf("counter = %lld\n", counter);
    }
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

bool Nonogram::Generator1D::attach(void (*const callback)(Nonogram::Cell *line, size_t line_sz))
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
#if DEBUG
    char char128[128];
    printf("[CALLED: depth = %d, start = %d, block_num = %d] ", depth, start - line, block_num);
    print();
    printf("\n");
    gets(char128);
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

Nonogram::Nonogram(const Array<Array<int>> rows, const Array<Array<int>> cols)
    : rows{ rows }, cols{ cols }
{    
}

Nonogram Nonogram::scan(const char *const file_name)
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
    int t = 0;

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
            t = 0;
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
            t = 0;
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

Nonogram::Solver Nonogram::mksolver() const
{
    if (isWellFormed())
        return Solver{ rows, cols };
    else
        throw Exception("***Nonogram::mksolver(): puzzle not well formed\n");
}

Nonogram::Solver::Solver(const Array<Array<int>> rows, const Array<Array<int>> cols)
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
    run(board, num, 0, 0, 0);
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

int Nonogram::Solver::run(Nonogram::Cell *const start_point, const int depth, const int i, const int block_num, const int combo)
{
#if DEBUG
    char char128[128];
    printf("[CALLED: start_point = %d, depth = %d, i = %d, block_num = %d]\n", start_point - board, depth, i, block_num);
    print();
    printf("\n");
    gets(char128);
#endif

    if (depth == 0) {
        if (isAnswer())
            solutions.push_back(Board{toMatrix()});

        return 0;
    }
    else {
        const int block_sz = rows[i][block_num], next_depth = depth - 1;
        Cell *left = start_point, *right = start_point, *next_floor = nullptr;
        int next_i = i, next_block_num = block_num + 1, t = 0, res = 0;

        if (rows[i].size() == 1 && rows[i][0] == 0)
            return run(&board[(i + 1) * n], next_depth, i + 1, 0, combo + 1);

        if (left + block_sz > &board[(i + 1) * n])
            return 1;

        if (block_num + 1 == rows[i].size()) {
            next_i = i + 1;
            next_floor = &board[(i + 1) * n];
            next_block_num = 0;
        }

        for (int b = 0; b < block_sz; b++)
            *right++ = BLACK;
            
        if (next_floor == nullptr)
            t = run(right + 1, next_depth, next_i, next_block_num, combo + 1);
        else
            t = run(next_floor, next_depth, next_i, next_block_num, combo + 1);

        switch (t) {
        case 0:
            while (right < &board[(i + 1) * n]) {
                *left++ = WHITE;
                *right++ = BLACK;
                if (next_floor == nullptr)
                    t = run(right + 1, next_depth, next_i, next_block_num, 1);
                else
                    t = run(next_floor, next_depth, next_i, next_block_num, 1);
                if (t)
                    break;
            }
        case 1:
            while (right > left)
                *--right = WHITE;
            return 0;
        default:
            while (right > left)
                *--right = WHITE;
            if (t > combo)
                return combo + 1;
            else
                return 1;
        }
    }
}
