#include "scratch.hpp"

static char hex2char(int hex);

SerialPrinter sout = { .prefix = "arduino> " };
SerialPrinter serr = { .prefix = "WARNING> " };

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
    std::stringstream ss{ };
    for (std::size_t i = 0; i < l; i++)
        ss << nums[i] << ' ';
    ss << '\n';
    return ss.str();
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

SerialPrinter SerialPrinter::operator<<(const byte x)
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

static char hex2char(const int hex)
{
    switch (hex % 16) {
    case 0:
        return '0';
    case 1:
        return '1';
    case 2:
        return '2';
    case 3:
        return '3';
    case 4:
        return '4';
    case 5:
        return '5';
    case 6:
        return '6';
    case 7:
        return '7';
    case 8:
        return '8';
    case 9:
        return '9';
    case 10:
        return 'A';
    case 11:
        return 'B';
    case 12:
        return 'C';
    case 13:
        return 'D';
    case 14:
        return 'E';
    case 15:
        return 'F';
    default:
        return '\0';
    }
}
