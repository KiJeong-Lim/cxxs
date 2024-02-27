#include "scratch.hpp"

static void scratch(void);

int main(void)
{
    scratch();
    return 0;
}

void scratch()
{
    int menu = 0;
    std::string prompt = "";
    std::cout << "<><> Project: scratch <><><><><><><>\n";
    std::cout << std::endl;
    std::cout << "[Menu]\n";
    std::cout << "\n";
    std::cout << "#1 NonogramSolver\n";
    std::cout << "\n";
    std::cout << "Enter the number of the menu: ";
    std::getline(std::cin, prompt);
    std::stringstream{ prompt } >> menu;
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
