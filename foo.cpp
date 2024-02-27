#include "scratch.hpp"

class Foo {
public:
    Foo()
    {
        std::cout << "Foo::Foo()\n";
    }
    ~Foo()
    {
        std::cout << "Foo::~Foo()\n";
    }
    Foo(const Foo &other)
    {
        std::cout << "Foo::Foo(const Foo &other)\n";
    }
    Foo(Foo &&other)
    {
        std::cout << "Foo::Foo(Foo &&other)\n";
    }
    Foo &operator=(Foo &&rhs)
    {
        std::cout << "Foo &Foo::operator=(Foo &&rhs)\n";
        return *this;
    }
    static Foo mkFoo(void)
    {
        return Foo{ };
    }
};

void test::foo()
{
    return;
}
