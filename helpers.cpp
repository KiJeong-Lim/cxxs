#include "scratch.hpp"

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
