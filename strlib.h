#ifndef STRLIB
#define STRLIB

#include <string>
#include <sstream>
#include <concepts>
#include <iostream>
#include "slice.h"

namespace strlib
{
    std::string to_str(bool);

    template <typename T>
    concept Printable = requires(std::ostream &os, T var) {
        {
            os << var
        } -> std::same_as<std::ostream &>;
    };

    template <Printable Item, Printable... Items>
    std::string format(const std::string &str, const Item &arg, const Items... args)
    {
        // gradually extract from str, when finding two brackets then insert the arg instead
        const long length{static_cast<long>(str.length())};
        std::ostringstream result{};
        for (int i = 0; i < length; ++i)
        {
            if (str[i] == '{' && i + 1 < length && str[i + 1] == '}')
            {
                result << arg;
                // add remaining args, skipping over closing bracket
                auto sliced{itertools::slice(str, i + 2, length)};
                std::string remaining(sliced.begin(), sliced.end());
                if constexpr (sizeof...(args) > 0)
                    result << strlib::format(remaining, args...);
                else
                    result << remaining;
                break;
            }
            else
                result << str[i];
        }
        return result.str();
    }
}

#endif
