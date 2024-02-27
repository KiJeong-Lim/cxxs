#include "scratch.hpp"

static int getMenu(void);
static void scratch(void);
static void greeting(void);

int main(void)
{
    greeting();
    scratch();
    return 0;
}

int getMenu()
{
    int result = 0;
    std::string menu{ };
    std::cout << "[Menu]\n";
    std::cout << "\n";
    std::cout << "#1 NonogramSolver\n";
    std::cout << "\n";
    std::cout << "Enter the number of the menu: ";
    std::getline(std::cin, menu);
    std::stringstream{ std::move(menu) } >> result;
    return result;
}

void scratch()
{
    const int menu = getMenu();
    switch (menu) {
    case 1:
        prog::solveNonogram();
        break;
    default:
        std::cout << "No menu selected...\n";
        break;
    }
    std::cout << std::endl;
    press_any_key();
}

void greeting()
{
    std::cout << "<><> Project: scratch <><><><><><><>\n";
    std::cout << std::endl;
}
