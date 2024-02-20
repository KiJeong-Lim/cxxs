#include "scratch.hpp"

#define DEBUG 0

template <typename A> using Array = std::vector<A>;
using Generator1D = NonogramSolver::Generator1D;

static void debug_Generator1D_callback(Generator1D::cell_t *line, size_t sz);
static long long int counter = 0;

void test_nonogramsolver()
{
    NonogramSolver solver = {};
    Array<NonogramSolver::Answer> solutions = {};
    int n = 0;

    solver.scanPuzzle("nonogramtest.txt");
    solver.solve();
    solutions = solver.getSolutions();
    n = solutions.size();

    for (int i = 0; i < n; i++) {
        solutions[i].print();
    }
}

void debug_Generator1D_callback(Generator1D::cell_t *const line, const size_t line_sz)
{
    printf("================\n");
    printf("0123456789ABCDEF\n");
    for (int i = 0; i < line_sz; i++) {
        printf("%d", line[i]);
    }
    printf("\n");
    printf("================\n");
    printf("\n");
    counter++;
}

void test_nonogramsolverlogic()
{
    Generator1D gen = {};
    int info[] = { 2, 2, 3, 1, };
    Generator1D::cell_t line[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
    bool well_formed = gen.init(line, len(line), info, len(info));
    
    if (well_formed) {
        gen.attach(debug_Generator1D_callback);
        gen.exec();
        printf("counter = %lld\n", counter);
    }
}

Generator1D::Generator1D()
    : callback{ nullptr }, line{ nullptr }, line_sz{ 0 }
{
}

void Generator1D::exec()
{
    int block_cnt = 0;
    bool last_was_black = false;

    for (int i = 0; i < line_sz; i++) {
        line[i] = WHITE;
    }

    run(line, info_sz, 0, 0);
}

bool Generator1D::attach(void (*const callback)(Generator1D::cell_t *line, size_t line_sz))
{
    if (callback == nullptr) {
        return false;
    }

    this->callback = callback;
    return true;
}

void Generator1D::print() const
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

bool Generator1D::init(Generator1D::cell_t *const line, const std::size_t line_sz, int *const info, const std::size_t info_sz)
{
    this->line_sz = line_sz;
    this->line = line;
    this->info = info;
    this->info_sz = info_sz;

    return true;
}

int Generator1D::run(Generator1D::cell_t *const start, const int depth, const int block_num, const int combo)
{
#if DEBUG == 2
    char char128[128];
    print();
    gets(char128);
#endif

#if DEBUG
    printf("[CALLED: depth = %d, start = %d] ", depth, start - line);
    print();
    printf("\n");
#endif

    if (depth == 0) {
        callback(line, line_sz);
        return 0;
    }
    else {
        const int block_sz = info[block_num];
        cell_t *left = start, *right = start;
        int go = combo, t = 0, res = 0;

        if (left + block_sz > line + line_sz) {
            return 1;
        }
        
        for (int b = 0; b < block_sz; b++) {
            *right++ = BLACK;
        }
        
        t = run(right + 1, depth - 1, block_num + 1, combo + 1);

        switch (t) {
        case 0:
            while (right < line + line_sz) {
                *left++ = WHITE;
                *right++ = BLACK;
                t = run(right + 1, depth - 1, block_num + 1, 0);
                if (t > 0) {
                    break;
                }
            }
        case 1:
            while (right > left)
                *--right = WHITE;
            break;
        default:
            if (t >= combo)
                res = combo + 1;
            else
                res = 1;
            while (right > left)
                *--right = WHITE;
        }
        return res;
    }
}

NonogramSolver::Exception::Exception(const char *const err_msg)
    : err_msg{ err_msg }
{
}

const char *NonogramSolver::Exception::what() const throw ()
{
    return err_msg.c_str();
}

NonogramSolver::~NonogramSolver()
{
    clear();
}

NonogramSolver::Answer::Answer(Array<Array<Cell>> board)
    : board{ board }
{
}

void NonogramSolver::Answer::print() const
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

bool NonogramSolver::setPuzzle(const Array<Array<int>> &rows, const Array<Array<int>> &cols)
{
    const int m = rows.size(), n = cols.size();
    bool well_formed = false;

    this->rows = rows;
    this->cols = cols;
    this->m = m;
    this->n = n;

    well_formed = puzzleWellFormed();

    if (well_formed) {
        this->board = new Cell [m * n];
        for (int i = 0; i < m * n; i++)
            board[i] = WHITE;
    }
    else
        this->board = nullptr;

    return well_formed;
}

bool NonogramSolver::puzzleWellFormed() const
{
    const int m = rows.size(), n = cols.size();
    int t = 0;

    if (m != this->m || n != this->n)
        return false;
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

Array<Array<NonogramSolver::Cell>> NonogramSolver::toMatrix() const
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

void NonogramSolver::print() const
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

bool NonogramSolver::scanPuzzle(const char *const file_name)
{
    Array<Array<int>> rows, cols;
    std::ifstream file;
    std::string line;
    bool okay = false;

    try {
        board = nullptr;
        file.open(file_name, std::ios::in);
        if (file.bad()) {
            throw Exception("***scanPuzzle: cannot open the file\n");
        }
        getline(file, line);
        if (line != std::string{ "rows" }) {
            throw Exception("***scanPuzzle: expected \'rows\'\n");
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
        okay = setPuzzle(rows, cols);
    }
    catch (const std::exception &e) {
        std::cerr << e.what();
        file.close();
    }

    return okay;
}

void NonogramSolver::clear()
{
    delete board;
    board = nullptr;
    m = 0;
    n = 0;
    rows.clear();
    cols.clear();
    solutions.clear();
}

const Array<NonogramSolver::Answer> &NonogramSolver::getSolutions() const
{
    return solutions;
}

void NonogramSolver::solve()
{
    int num = 0;
    for (int i = 0; i < m; i++)
        num += rows[i].size();
    run(board, num, 0, 0);
}

bool NonogramSolver::isAnswer()
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

int NonogramSolver::run(NonogramSolver::Cell *const start_point, const int depth, const int i, const int block_num)
{
#if DEBUG
    char char128[128];
    printf("[CALLED: start_point = %d, depth = %d, i = %d, block_num = %d]\n", start_point - board, depth, i, block_num);
    printBoard();
    printf("\n");
    gets(char128);
#endif

    if (depth == 0) {
        if (isAnswer())
            solutions.push_back(Answer{toMatrix()});

        return 1;
    }
    else {
        const int block_sz = rows[i][block_num], next_depth = depth - 1;
        Cell *left = start_point, *right = start_point, *next_floor = nullptr;
        int next_i = i, next_block_num = block_num + 1;

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

        if (next_floor == nullptr)
            while (run(right + 1, next_depth, next_i, next_block_num)) {
                if (right >= &board[(i + 1) * n])
                    break;
                *left++ = WHITE;
                *right++ = BLACK;
            }
        else
            while (run(next_floor, next_depth, next_i, next_block_num)) {
                if (right >= &board[(i + 1) * n])
                    break;
                *left++ = WHITE;
                *right++ = BLACK;
            }

        while (right > left)
            *--right = WHITE;

        return 1;
    }
}

NonogramSolver::Cell &NonogramSolver::at(const int i, const int j)
{
    return board[i * n + j];
}

const NonogramSolver::Cell &NonogramSolver::at(const int i, const int j) const
{
    return board[i * n + j];
}
