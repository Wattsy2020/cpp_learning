#ifndef STRLIB
#define STRLIB

#include <string>
#include <sstream>

namespace strlib
{
    std::string to_str(bool);

    std::string to_str(std::stringstream stream);

    // TODO: make it take a variable number of arguments
    // TODO: use concepts to specify that T must be convertable to string
    template <typename T>
    std::string format(const std::string &str, const T &arg)
    {
        if (str.empty())
            return str;

        // gradually extract from str, when finding two brackets then insert the arg instead
        std::stringstream result{};
        for (int i = 0; i < str.length(); ++i)
        {
            if (str[i] == '{' && i + 1 < str.length() && str[i + 1] == '}')
            {
                result << arg;
                ++i; // skip over closing bracket
            }
            else
                result << str[i];
        }
        return to_str(std::move(result));
    }
}

#endif
