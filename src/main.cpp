#include "scratch.hpp"

int main(void)
{
    return 0;
}

void scratch::main()
{
    int menu = 0;
    std::string prompt = "";

    std::cout << "<><> Project: scratch <><><><><><><>\n";
    std::cout << std::endl;
    std::cout << "[Menu]\n";
    std::cout << "\n";
    std::cout << "#1 Solve nonogram\n";
    std::cout << "#2 Solve 15-puzzle\n";
    std::cout << "\n";
    std::cout << "Enter the number of the menu: ";
    std::cout.flush();
    std::getline(std::cin, prompt);
    if (std::stringstream{ prompt } >> menu) {
        switch (menu) {
        case 1:
            scratch::solvenonogram();
            break;
        case 2:
            scratch::fifteenpuzzle();
            break;
        default:
            std::cout << "No menu selected...\n";
            break;
        }
        std::cout << std::endl;
    }
    else
        std::cout << "Bad input...\n";
    press_any_key();
}
