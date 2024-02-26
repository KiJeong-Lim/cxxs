#include "scratch.hpp"

#define DEBUG 0

template <typename ELEM> using Array = std::vector<ELEM>;

static void debug_Generator1D_callback(const Nonogram::Cell *line, std::size_t sz);
static unsigned long long int generator1d_callback_call_count = 0;

void test_nonogramsolver()
{
    try {
        auto puzzle = Nonogram::scanPuzzle("nonogramtest.txt");
        auto solver = puzzle.mkSolver();
        auto solutions = solver.solve();
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
        auto puzzle = Nonogram::scanPuzzle("nonogramtest.txt");
        auto solver = puzzle.mkSolverV2();
        auto solution = solver.solve();
        std::cout << solution;
    }
    catch (const std::exception &e) {
        std::cerr << e.what();
    }
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
    Nonogram::Cell line[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
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

Nonogram::Generator1D::Generator1D()
    : callback{ nullptr }, line{ nullptr }, line_sz{ 0 }
{
}

void Nonogram::Generator1D::exec()
{
    for (std::size_t i = 0; i < line_sz; i++)
        line[i] = WHITE;

    run(line, info_sz, 0);
}

bool Nonogram::Generator1D::attach(void (*const callback)(const Nonogram::Cell *line, std::size_t line_sz))
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
    this->line = line;
    this->info = info;
    this->line_sz = line_sz;
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
    for (std::size_t i = 0; i < board.size(); i++) {
        for (std::size_t j = 0; j < board[i].size(); j++) {
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
    for (std::size_t i = 0; i < m; i++)
        if (rows[i].size() == 0)
            return false;
    for (std::size_t j = 0; j < n; j++)
        if (cols[j].size() == 0)
            return false;
    for (std::size_t i = 0; i < m; i++)
        if (rows[i].size() == 1 && rows[i][0] == 0)
            continue;
        else {
            int t = 0;
            for (std::size_t k = 0; k < rows[i].size(); k++)
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
    for (std::size_t j = 0; j < n; j++)
        if (cols[j].size() == 1 && cols[j][0] == 0)
            continue;
        else {
            int t = 0;
            for (std::size_t k = 0; k < cols[j].size(); k++)
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
        return Solver{ .rows = rows, .cols = cols };
    else
        throw Exception("***Nonogram::mkSolver(): puzzle not well formed\n");
}

Nonogram::SolverV2 Nonogram::mkSolverV2() const
{
    if (isWellFormed()) {
        Array<Array<int>> rows = this->rows, cols = this->cols;
        for (std::size_t i = 0; i < rows.size(); i++) {
            if (rows[i].size() == 1 && rows[i][0] == 0)
                rows[i] = std::vector<int>{ };
        }
        for (std::size_t j = 0; j < cols.size(); j++) {
            if (cols[j].size() == 1 && cols[j][0] == 0)
                cols[j] = std::vector<int>{ };
        }
        return SolverV2{ .rows = rows, .cols = cols };
    }
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

const Array<Nonogram::Board> &Nonogram::Solver::solve()
{
    int num = 0;
    for (std::size_t i = 0; i < m; i++)
        num += rows[i].size();
    for (std::size_t i = 0; i < m * n; i++)
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
    for (std::size_t i = 0; i < m; i++) {
        for (std::size_t j = 0; j < n; j++) {
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
    delete[] board;
    board = nullptr;
}

bool Nonogram::Solver::isAnswer()
{
    int black_cnt = 0, k = 0;

    for (std::size_t j = 0; j < n; j++) {
        black_cnt = 0;
        k = 0;

        for (std::size_t i = 0; i < m; i++)
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
    for (std::size_t i = 0; i < m; i++) {
        Array<Cell> line = {};
        for (std::size_t j = 0; j < n; j++)
            line.push_back(at(i, j));
        ret.push_back(std::move(line));
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

Nonogram::SolverV2::Generator::Generator(const std::size_t line_sz, const Array<int> &info)
    : line{ nullptr }, line_sz{ line_sz }, info{ info }
{
    line = new Cell [line_sz];
    for (std::size_t i = 0; i < line_sz; i++)
        line[i] = WHITE;
}

Nonogram::SolverV2::Generator::~Generator()
{
    delete[] line;
    line = nullptr;
}

bool Nonogram::SolverV2::Generator::accumulatePossiblities(Nonogram::Cell *const start_point, const std::size_t block_num, const Array<Nonogram::SolverV2::Value_t> &line_ref, Array<Array<Nonogram::Cell>> &accum)
{
    if (block_num == info.size()) {
        bool is_compatible = true;

        for (std::size_t i = 0; i < line_sz; i++) {
            switch (line_ref[i]) {
            case Unknown:
                break;
            case Sharp:
                is_compatible &= (line[i] == BLACK);
                break;
            case Empty:
                is_compatible &= (line[i] == WHITE);
                break;
            default:
                is_compatible = false;
                break;
            }
        }

        if (is_compatible)
            accum.push_back(std::vector<Cell>(line, line + line_sz));

        return true;
    }
    else {
        const int block_sz = info[block_num];
        Cell *left = start_point, *right = start_point;

        if (start_point + block_sz > line + line_sz)
            return false;

        for (int b = 0; b < block_sz; b++)
            *right++ = BLACK;

        while (accumulatePossiblities(right + 1, block_num + 1, line_ref, accum)) {
            if (right >= line + line_sz)
                break;
            *left++ = WHITE;
            *right++ = BLACK;
        }

        while (right > left)
            *--right = WHITE;

        return true;
    }
}

Array<Array<Nonogram::Cell>> Nonogram::SolverV2::Generator::findAllPossiblitiesCompatibleWith(const Array<Nonogram::SolverV2::Value_t> &reference)
{
    Array<Array<Cell>> possiblities = {};
    accumulatePossiblities(line, 0, reference, possiblities);
    return possiblities;
}

Array<Nonogram::SolverV2::Value_t> Nonogram::SolverV2::solveLine(const Array<Nonogram::SolverV2::Value_t> &line, const Array<int> &info)
{
    Generator generator{ .line_sz = line.size(), .info = info };
    const Array<Array<Cell>> possiblities = generator.findAllPossiblitiesCompatibleWith(line);
    Array<Value_t> new_line = {};

    for (std::size_t i = 0; i < line.size(); i++) {
        if (line[i] == Unknown) {
            Array<Cell> xs_i = {};
            for (std::size_t k = 0; k < possiblities.size(); k++)
                xs_i.push_back(possiblities[k][i]);
            if (xs_i == std::vector<Cell>(xs_i.size(), BLACK))
                new_line.push_back(Sharp);
            else if (xs_i == std::vector<Cell>(xs_i.size(), WHITE))
                new_line.push_back(Empty);
            else
                new_line.push_back(Unknown);
        }
        else
            new_line.push_back(line[i]);
    }

    return new_line;
}

bool Nonogram::SolverV2::checkRow(const std::size_t i)
{
    bool has_changed = false;
    Array<Value_t> line = {};

    if (!rows_done[i]) {
        bool done = true;
        for (std::size_t j = 0; j < n; j++)
            line.push_back(at(i, j));
        const Array<Value_t> new_line = solveLine(line, rows[i]);
        if (new_line != line) {
            has_changed = true;
            for (std::size_t j = 0; j < n; j++)
                at(i, j) = new_line[j];
        }
        for (std::size_t j = 0; j < n; j++)
            done &= (at(i, j) == Sharp || at(i, j) == Empty);
        rows_done[i] = done;
    }

    return has_changed;
}

bool Nonogram::SolverV2::checkCol(const std::size_t j)
{
    bool has_changed = false;
    Array<Value_t> line = {};

    if (!cols_done[j]) {
        bool done = true;
        for (std::size_t i = 0; i < m; i++)
            line.push_back(at(i, j));
        const Array<Value_t> new_line = solveLine(line, cols[j]);
        if (new_line != line) {
            has_changed = true;
            for (std::size_t i = 0; i < m; i++)
                at(i, j) = new_line[i];
        }
        for (std::size_t i = 0; i < m; i++)
            done &= (at(i, j) == Sharp) || (at(i, j) == Empty);
        cols_done[j] = done;
    }

    return has_changed;
}

std::string Nonogram::SolverV2::solve()
{
    bool changed = true, solved = true;

    while (changed) {
        changed = false;
        for (std::size_t i = 0; i < m; i++)
            changed |= checkRow(i);
        for (std::size_t j = 0; j < n; j++)
            changed |= checkCol(j);
    }

    for (std::size_t i = 0; i < m; i++)
        solved &= rows_done[i];
    for (std::size_t j = 0; j < n; j++)
        solved &= cols_done[j];

    if (solved) {
        std::stringstream ss{ };
        for (std::size_t i = 0; i < m; i++) {
            for (std::size_t j = 0; j < n; j++)
                ss << static_cast<char>(at(i, j));
            ss << '\n';
        }
        ss << '\n';
        return ss.str();
    }
    else
        return std::string{ "***Nonogram::SolverV2::solve(): cannot solve the puzzle\n" };
}

Nonogram::SolverV2::SolverV2(const Array<Array<int>> &rows, const Array<Array<int>> &cols)
    : rows{ rows }, cols{ cols }, m{ rows.size() }, n{ cols.size() }, rows_done{ std::vector<bool>(m, false) }, cols_done{ std::vector<bool>(n, false) }, board{ nullptr }
{
    board = new Value_t [m * n];
    for (std::size_t i = 0; i < m * n; i++)
        board[i] = Unknown;
}

Nonogram::SolverV2::~SolverV2()
{
    delete[] board;
    board = nullptr;
}
