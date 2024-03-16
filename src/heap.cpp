#include "scratch.hpp"

void test::heap()
{
    MaxHeap<int, 16> xs{ };

    xs.push(1);
    xs.push(2);
    xs.push(5);
    xs.push(6);
    xs.push(3);
    xs.push(4);

    while (!xs.isEmpty()) {
        std::cout << xs.pop() << std::endl;
    }
}
