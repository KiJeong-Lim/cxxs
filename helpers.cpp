#include "scratch.hpp"

std::vector<int> IntegerHelper::reads(const std::string &str)
{
    std::stringstream ss = static_cast<std::stringstream &&>(std::stringstream{} << str);
    std::vector<int> xs = {};
    std::string s = "";
    int x = 0;

    while (!ss.eof()) {
        ss >> s;
        if (std::stringstream(s) >> x)
            xs.push_back(x);
        s = "";
    }

    return xs;
}

void IntegerHelper::shows(const std::vector<int> &ns)
{
    const std::size_t l = ns.size();
    for (std::size_t i = 0; i < l; i++)
        std::cout << ns[i] << ' ';
    std::cout << std::endl;
}
