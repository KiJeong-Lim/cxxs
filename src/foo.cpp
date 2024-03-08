#include "scratch.hpp"

class Foo {
    std::string id;
public:
    Foo()
        : id{ "" }
    {
        std::cout << "Foo::Foo()\n";
    }
    ~Foo()
    {
        std::cout << "Foo::~Foo()\n";
    }
    Foo(const Foo &other)
        : id{ other.id }
    {
        std::cout << "Foo::Foo(const Foo &other)\n";
    }
    Foo(Foo &&other)
        : id{ other.id }
    {
        std::cout << "Foo::Foo(Foo &&other)\n";
    }
    Foo &operator=(Foo &&rhs)
    {
        id = rhs.id;
        std::cout << "Foo &Foo::operator=(Foo &&rhs)\n";
        return *this;
    }
    Foo(const std::string &id)
        : id{ id }
    {
        std::cout << "Foo::Foo(" << id << ")\n";
    }
    static Foo mkFoo(void)
    {
        return Foo{ };
    }
};

void test::foo()
{
    Foo foo1{ "foo1" };
    return;
}
