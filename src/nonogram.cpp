#include "scratch.hpp"

#define DEBUG 1

template <typename ELEM> using Array = std::vector<ELEM>;

void prog::solvenonogram()
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
        getline(file, line);
        if (line == std::string{ "" }) {
            throw Exception{ "***Nonogram::scanPuzzle(): cannot open the file...\n" };
        }
        if (line != std::string{ "rows" }) {
            throw Exception{ "***Nonogram::scanPuzzle(): expected \'rows\'...\n" };
        }
        while (getline(file, line)) {
            if (line == std::string{ "cols" })
                break;
            rows.push_back(IntegerHelper::scanIntegers(line));
        }
        while (getline(file, line)) {
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
    catch (const std::exception &e) {
        file.close();
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
            if (int2size_t(t) > m)
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
            if (int2size_t(t) > n)
                return false;
        }
    return true;
}

Nonogram::Solver Nonogram::mkSolver() const
{
    if (isWellFormed())
        return Solver{ .rows = rows, .cols = cols };
    else
        throw Exception{ "***Nonogram::mkSolver(): puzzle ill-formed...\n" };
}

Nonogram::Solver::Solver(const Array<Array<int>> &rows, const Array<Array<int>> &cols)
    : m{ rows.size() }, n{ cols.size() }, rows{ rows }, cols{ cols }, row_dones{ Array<bool>(m, false) }, col_dones{ Array<bool>(n, false) }, board{ nullptr }
{
    board = new Cell [m * n];
    for (std::size_t i = 0; i < m * n; i++) {
        board[i] = UNKNOWN;
    }
}

Nonogram::Solver::~Solver()
{
    clear();
}

std::string Nonogram::Solver::solve()
{
    bool has_changed = true;
    bool all_done = true;

    while (has_changed) {
        has_changed = false;
        for (std::size_t i = 0; i < m; i++)
            has_changed |= traverseRow(i, rows[i]);
        for (std::size_t j = 0; j < n; j++)
            has_changed |= traverseCol(j, cols[j]);
    }

    for (std::size_t i = 0; i < m; i++)
        all_done &= row_dones[i];
    for (std::size_t j = 0; j < n; j++)
        all_done &= col_dones[j];

    if (all_done) {
        std::stringstream res{ };
        for (std::size_t i = 0; i < m; i++) {
            for (std::size_t j = 0; j < n; j++) {
                switch(at(i, j)) {
                case NEITHER:
                    res << ' ';
                    break;
                case EMPTY:
                    res << '.';
                    break;
                case STAR:
                    res << '#';
                    break;
                case UNKNOWN:
                    res << '?';
                    break;
                default:
                    res << 'x';
                }
            }
            res << '\n';
        }
        res << '\n';
        return res.str();
    }
    else
        return std::string{ "cannot solve the puzzle..." };
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

bool Nonogram::Solver::traverseRow(const std::size_t i, const Array<int> &row)
{
    bool has_changed = false, all_done = true;
    if (!row_dones[i]) {
        int go = 0, combo = 0, z = 0;
        Cell *const line = new int [n], *const result = new int [n], **const ptr_list = new int *[row.size()];
        for (std::size_t j = 0; j < n; j++) {
            line[j] = EMPTY;
            result[j] = at(i, j);
        }
        for (std::size_t k = 0; k < row.size(); k++) {
            ptr_list[k] = nullptr;
        }
        for (go = 0; z >= 0; go /= 2) {
            switch (go) {
            case 0:
                combo++;
                if (int2size_t(z) == row.size()) {
                    bool invalid = false;
                    for (std::size_t j = 0; j < n; j++) {
                        if ((line[j] & at(i, j)) == NEITHER) {
                            invalid = true;
                            break;
                        }
                    }
                    if (!invalid) {
                        for (std::size_t j = 0; j < n; j++) {
                            result[j] &= line[j];
                        }
                    }
                }
                else {
                    ptr_list[z] = z == 0 ? line : ptr_list[z - 1] + 1;
                    for (int k = 0; k < row[z]; k++) {
                        *ptr_list[z]++ = STAR;
                    }
                    z++;
                    break;
                }
                while (--z >= 0) {
            default:
                    if (go != 2 && ptr_list[z] < line + n) {
                        ptr_list[z][-row[z]] = EMPTY;
                        *ptr_list[z]++ = STAR;
                        combo = 0;
                        z++;
                        break;
                    }
                    else {
                        go = 2;
                        if (combo == 0) {
                            break;
                        }
                        combo--;
                        for (int k = 0; k < row[z]; k++) {
                            *--ptr_list[z] = EMPTY;
                        }
                    }
                }
            }
        }
        for (std::size_t j = 0; j < n; j++) {
            if (result[j] != NEITHER && at(i, j) == UNKNOWN) {
                at(i, j) = result[j];
                has_changed = true;
            }
        }
        delete[] line;
        delete[] result;
        delete[] ptr_list;
    }
    for (std::size_t j = 0; j < n; j++) {
        all_done &= (at(i, j) != UNKNOWN);
    }
    if (all_done)
        row_dones[i] = true;
    return has_changed;
}

bool Nonogram::Solver::traverseCol(const std::size_t j, const Array<int> &col)
{
    bool has_changed = false, all_done = true;
    if (!col_dones[j]) {
        int go = 0, combo = 0, z = 0;
        Cell *const line = new int [m], *const result = new int [m], **const ptr_list = new int *[col.size()];
        for (std::size_t i = 0; i < m; i++) {
            line[i] = EMPTY;
            result[i] = at(i, j);
        }
        for (std::size_t k = 0; k < col.size(); k++) {
            ptr_list[k] = nullptr;
        }
        for (go = 0; z >= 0; go /= 2) {
            switch (go) {
            case 0:
                combo++;
                if (int2size_t(z) == col.size()) {
                    bool invalid = false;
                    for (std::size_t i = 0; i < m; i++) {
                        if ((line[i] & at(i, j)) == NEITHER) {
                            invalid = true;
                            break;
                        }
                    }
                    if (!invalid) {
                        for (std::size_t i = 0; i < m; i++) {
                            result[i] &= line[i];
                        }
                    }
                }
                else {
                    ptr_list[z] = z == 0 ? line : ptr_list[z - 1] + 1;
                    for (int k = 0; k < col[z]; k++) {
                        *ptr_list[z]++ = STAR;
                    }
                    z++;
                    break;
                }
                while (--z >= 0) {
            default:
                    if (go != 2 && ptr_list[z] < line + m) {
                        ptr_list[z][-col[z]] = EMPTY;
                        *ptr_list[z]++ = STAR;
                        combo = 0;
                        z++;
                        break;
                    }
                    else {
                        go = 2;
                        if (combo == 0) {
                            break;
                        }
                        combo--;
                        for (int k = 0; k < col[z]; k++) {
                            *--ptr_list[z] = EMPTY;
                        }
                    }
                }
            }
        }
        for (std::size_t i = 0; i < m; i++) {
            if (result[i] != NEITHER && at(i, j) == UNKNOWN) {
                at(i, j) = result[i];
                has_changed = true;
            }
        }
        delete[] line;
        delete[] result;
        delete[] ptr_list;
    }
    for (std::size_t i = 0; i < m; i++) {
        all_done &= (at(i, j) != UNKNOWN);
    }
    if (all_done)
        col_dones[j] = true;
    return has_changed;
}
