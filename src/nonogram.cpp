#include "scratch.hpp"

#define DEBUG 1

void scratch::solvenonogram()
{
    std::string file_name{ };

    std::cout << "Enter the file name of the Nonogram puzzle: ";
    std::cout.flush();
    std::cin >> file_name;
    try {
        auto puzzle = Nonogram::scanPuzzle(file_name.c_str());
        auto solver = puzzle.mkSolver();
        auto solution = solver.solve();

        std::cout << std::endl;
        std::cout << solution;
    }
    catch (const std::exception &e) {
        std::cout << "Abort...\n";
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

Nonogram::Nonogram(const Array<Array<int>> &rows, const Array<Array<int>> &cols)
    : rows{ rows }, cols{ cols }
{
}

Nonogram Nonogram::scanPuzzle(const char *const file_name)
{
    Array<Array<int>> rows = {}, cols = {};
    std::ifstream file{ };
    std::string line{ };

    try {
        file.open(file_name, std::ios::in);
        if (file.bad()) {
            throw Exception{ "***Nonogram::scanPuzzle(): cannot open the file...\n" };
        }
        while (std::getline(file, line)) {
            if (line == std::string{ "rows" })
                break;
        }
        while (std::getline(file, line)) {
            if (line == std::string{ "cols" })
                break;
            rows.push_back(IntegerHelper::scanIntegers(line));
        }
        while (std::getline(file, line)) {
            if (line == std::string{ "end" })
                break;
            cols.push_back(IntegerHelper::scanIntegers(line));
        }
        file.close();
        return Nonogram{ .rows = rows, .cols = cols };
    }
    catch (const Nonogram::Exception &e) {
        file.close();
        std::cout << e.what();
        throw e;
    }
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
        if (rows[i][0] == 0) {
            if (rows.size() == 1)
                continue;
            else
                return false;
        }
        else {
            int t = 0;
            for (std::size_t k = 0; k < rows[i].size(); k++)
                if (rows[i][k] <= 0)
                    return false;
                else {
                    if (t == 0)
                        t = rows[i][k];
                    else
                        t += rows[i][k] + 1;
                }
            if (int2size_t(t) > m)
                return false;
        }
    for (std::size_t j = 0; j < n; j++)
        if (cols[j][0] == 0) {
            if (cols[j].size() == 1)
                continue;
            else
                return false;
        }
        else {
            int t = 0;
            for (std::size_t k = 0; k < cols[j].size(); k++)
                if (cols[j][k] <= 0)
                    return false;
                else {
                    if (t == 0)
                        t = cols[j][k];
                    else
                        t += cols[j][k] + 1;
                }
            if (int2size_t(t) > n)
                return false;
        }
    return true;
}

Nonogram::Solver Nonogram::mkSolver() const
{
    if (isWellFormed()) {
        Array<Array<int>> rows = this->rows, cols = this->cols;

        for (std::size_t i = 0; i < this->rows.size(); i++) {
            if (this->rows[i].size() == 1 && this->rows[i][0] == 0)
                rows[i] = Array<int>{ };
        }
        for (std::size_t j = 0; j < this->cols.size(); j++) {
            if (this->cols[j].size() == 1 && this->cols[j][0] == 0)
                cols[j] = Array<int>{ };
        }
        return Solver{ .rows = rows, .cols = cols };
    }
    else
        throw Exception{ "***Nonogram::mkSolver(): puzzle ill-formed...\n" };
}

Nonogram::Solver::Solver(const Array<Array<int>> &rows, const Array<Array<int>> &cols)
    : m{ rows.size() }, n{ cols.size() }, rows{ rows }, cols{ cols }, row_dones{ Array<bool>(m, false) }, col_dones{ Array<bool>(n, false) }, board{ nullptr }
{
    board = new Cell [m * n];
    for (std::size_t i = 0; i < m * n; i++)
        board[i] = INDETERMINED;
}

Nonogram::Solver::~Solver()
{
    clear();
}

std::string Nonogram::Solver::solve()
{
    bool has_changed = true, all_done = true;

    while (has_changed) {
        has_changed = false;
        for (std::size_t i = 0; i < m; i++)
            has_changed |= traverseRow(i);
        for (std::size_t j = 0; j < n; j++)
            has_changed |= traverseCol(j);
    }

    for (std::size_t i = 0; i < m; i++)
        all_done &= row_dones[i];
    for (std::size_t j = 0; j < n; j++)
        all_done &= col_dones[j];

    if (all_done) {
        std::stringstream res{ };

        for (std::size_t i = 0; i < m; i++) {
            for (std::size_t j = 0; j < n; j++)
                switch (at(i, j)) {
                case BOTHPOSSIBLE:
                    res << '_';
                    break;
                case WHITE:
                    res << '.';
                    break;
                case BLACK:
                    res << '#';
                    break;
                case INDETERMINED:
                    res << '?';
                    break;
                default:
                    res << 'X';
                }
            res << '\n';
        }
        res << '\n';
        return res.str();
    }
    else
        return std::string{ "cannot solve the puzzle...\n" };
}

void Nonogram::Solver::clear()
{
    delete[] board;
    board = nullptr;
}

Nonogram::Solver::Cell &Nonogram::Solver::at(const std::size_t i, const std::size_t j)
{
    return board[i * n + j];
}

bool Nonogram::Solver::traverseRow(const std::size_t i)
{
    bool has_changed = false, all_done = true;

    if (!row_dones[i]) {
        const Array<int> &row = rows[i];
        int combo = 0, s = 0, t = 0;
        Cell *const line = new Cell [n], *const result = new Cell [n], **const ptr_list = new Cell *[row.size()];

        for (std::size_t j = 0; j < n; j++) {
            line[j] = WHITE;
            result[j] = at(i, j);
        }
        for (std::size_t k = 0; k < row.size(); k++)
            ptr_list[k] = nullptr;
        for (s = 0; t >= 0; s /= 2)
            switch (s) {
            case 0:
                combo++;
                if (int2size_t(t) == row.size()) {
                    bool decidable = true;

                    for (std::size_t j = 0; j < n; j++)
                        if ((line[j] & at(i, j)) == BOTHPOSSIBLE) {                            
                            decidable = false;
                            break;
                        }
                    if (decidable)
                        for (std::size_t j = 0; j < n; j++)
                            result[j] &= line[j];
                }
                else {
                    ptr_list[t] = (t == 0 ? line : ptr_list[t - 1] + 1);
                    for (int k = 0; k < row[t]; k++)
                        *ptr_list[t]++ = BLACK;
                    t++;
                    break;
                }
                while (--t >= 0)
            default:
                    if (s != 2 && ptr_list[t] < line + n) {
                        ptr_list[t][-row[t]] = WHITE;
                        *ptr_list[t]++ = BLACK;
                        combo = 0;
                        t++;
                        break;
                    }
                    else {
                        s = 2;
                        if (combo == 0)
                            break;
                        combo--;
                        for (int k = 0; k < row[t]; k++)
                            *--ptr_list[t] = WHITE;
                    }
            }
        for (std::size_t j = 0; j < n; j++)
            if (result[j] != BOTHPOSSIBLE && at(i, j) == INDETERMINED) {
                at(i, j) = result[j];
                has_changed = true;
            }
        delete[] line;
        delete[] result;
        delete[] ptr_list;
        for (std::size_t j = 0; j < n; j++)
            all_done &= (at(i, j) != INDETERMINED);
        if (all_done)
            row_dones[i] = true;
    }
    return has_changed;
}

bool Nonogram::Solver::traverseCol(const std::size_t j)
{
    bool has_changed = false, all_done = true;

    if (!col_dones[j]) {
        const Array<int> &col = cols[j];
        int combo = 0, s = 0, t = 0;
        Cell *const line = new Cell [m], *const result = new Cell [m], **const ptr_list = new Cell *[col.size()];

        for (std::size_t i = 0; i < m; i++) {
            line[i] = WHITE;
            result[i] = at(i, j);
        }
        for (std::size_t k = 0; k < col.size(); k++)
            ptr_list[k] = nullptr;
        for (s = 0; t >= 0; s /= 2)
            switch (s) {
            case 0:
                combo++;
                if (int2size_t(t) == col.size()) {
                    bool decidable = true;

                    for (std::size_t i = 0; i < m; i++)
                        if ((line[i] & at(i, j)) == BOTHPOSSIBLE) {
                            decidable = false;
                            break;
                        }
                    if (decidable)
                        for (std::size_t i = 0; i < m; i++)
                            result[i] &= line[i];
                }
                else {
                    ptr_list[t] = (t == 0 ? line : ptr_list[t - 1] + 1);
                    for (int k = 0; k < col[t]; k++)
                        *ptr_list[t]++ = BLACK;
                    t++;
                    break;
                }
                while (--t >= 0)
            default:
                    if (s != 2 && ptr_list[t] < line + m) {
                        ptr_list[t][-col[t]] = WHITE;
                        *ptr_list[t]++ = BLACK;
                        combo = 0;
                        t++;
                        break;
                    }
                    else {
                        s = 2;
                        if (combo == 0)
                            break;
                        combo--;
                        for (int k = 0; k < col[t]; k++)
                            *--ptr_list[t] = WHITE;
                    }
            }
        for (std::size_t i = 0; i < m; i++)
            if (result[i] != BOTHPOSSIBLE && at(i, j) == INDETERMINED) {
                at(i, j) = result[i];
                has_changed = true;
            }
        delete[] line;
        delete[] result;
        delete[] ptr_list;
        for (std::size_t i = 0; i < m; i++)
            all_done &= (at(i, j) != INDETERMINED);
        if (all_done)
            col_dones[j] = true;
    }
    return has_changed;
}
