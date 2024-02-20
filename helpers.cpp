#include "scratch.hpp"

std::vector<int> IntegerHelper::reads(const std::string str)
{
    std::stringstream ss;
    std::vector<int> ret;
    std::string temp;
    int found;

    ss << str;
    while (!ss.eof()) {
        ss >> temp;
        if (std::stringstream(temp) >> found) {
            ret.push_back(found);
        }
        temp = "";
    }

    return ret;
}

void IntegerHelper::shows(std::vector<int> ns)
{
    const size_t sz = ns.size();
    for (int i = 0; i < sz; i++) {
        std::cout << ns[i] << ' ';
    }
    std::cout << std::endl;
}
