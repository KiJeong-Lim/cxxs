#include "scratch.hpp"

static void press_any_key(void);

int main(void)
{
    test::io();
    return 0;
}

void press_any_key()
{
    std::cout << "Press any key...\n";
    os.getch();
}
