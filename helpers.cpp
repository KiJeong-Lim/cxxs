#include "scratch.hpp"

static char hex2char(unsigned int hex);

SerialPrinter sout = { .prefix = "arduino> " };
SerialPrinter serr = { .prefix = "WARNING> " };

void test_serialprinter()
{
    sout << "hello" << ' ' << 1234 << ' ' << 3.141592;
    sout << "world" << ' ' << 5678 << ' ' << 2.718281;
}

std::vector<int> IntegerHelper::reads(const std::string &str)
{
    std::stringstream ss = static_cast<std::stringstream &&>(std::stringstream{ } << str);
    std::vector<int> xs = {};
    std::string s = "";

    while (!ss.eof()) {
        int x = 0;
        ss >> s;
        if (std::stringstream{ s } >> x)
            xs.push_back(x);
        s = "";
    }

    return xs;
}

std::string IntegerHelper::shows(const std::vector<int> &nums)
{
    const std::size_t l = nums.size();
    if (l == 0)
        return std::string{ "" };
    else {    
        std::stringstream ss{ };
        ss << nums[0];
        for (std::size_t i = 1; i < l; i++)
            ss << ' ' << nums[i];
        return ss.str();
    }
}

SerialPrinter::SerialPrinter(SerialPrinter &&other)
    : msg_prefix{ other.msg_prefix }, mknewline{ other.mknewline }
{
    other.mknewline = false;
}

SerialPrinter::SerialPrinter(const char *const prefix)
    : msg_prefix{ prefix }, mknewline{ false }
{
}

SerialPrinter::SerialPrinter(const char *const prefix, const bool lend)
    : msg_prefix{ prefix }, mknewline{ lend }
{
}

SerialPrinter::~SerialPrinter()
{
    if (mknewline)
        std::cout << '\n';
    os.delay(5);
}

void SerialPrinter::trick()
{
    mknewline = false;
    if (msg_prefix != nullptr)
        std::cout << msg_prefix;
}

SerialPrinter SerialPrinter::operator<<(const bool b)
{
    trick();
    std::cout << (b ? "true" : "false");
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const SerialPrinter::byte x)
{
    trick();
    std::cout << "0x" << hex2char(x / 16) << hex2char(x % 16);
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const int n)
{
    trick();
    std::cout << n;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const char *const s)
{
    trick();
    std::cout << s;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const double v)
{
    trick();
    std::cout << v;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const std::string &s)
{
    trick();
    std::cout << s;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const std::stringstream &ss)
{
    trick();
    std::cout << ss.str();
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

SerialPrinter SerialPrinter::operator<<(const char c)
{
    trick();
    std::cout << c;
    return SerialPrinter{ .prefix = nullptr, .lend = true };
}

char hex2char(const unsigned int hex)
{
    if (hex >= 16)
        return '\0';
    else
        return "0123456789ABCDEF"[hex];
}
