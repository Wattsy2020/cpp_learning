#ifndef STRLIB
#define STRLIB

#include <string>
#include <sstream>
#include <vector>
#include <concepts>
#include <iostream>
#include "slice.h"

namespace strlib
{
    std::string to_str(bool b)
    {
        return (b) ? "true" : "false";
    }

    template <typename T>
    concept Printable = requires(std::ostream &os, T var) {
        {
            os << var
        } -> std::same_as<std::ostream &>;
    };

    // split string by spaces
    std::vector<std::string> split(const std::string &str)
    {
        std::istringstream stream{str};
        std::vector<std::string> result{};
        std::string curr_word{};
        while (!stream.eof())
        {
            stream >> curr_word;
            result.push_back(curr_word);
        }
        return result;
    }

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

template <strlib::Printable T1, strlib::Printable T2>
std::ostream &operator<<(std::ostream &os, const std::tuple<T1, T2> &tuple)
{
    auto [first, second] = tuple;
    os << "(" << first << ", " << second << ")";
    return os;
}

template <strlib::Printable T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &optional)
{
    if (optional)
        os << optional.value();
    else
        os << "None";
    return os;
}

template <strlib::Printable T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vect)
{
    os << "[ ";
    for (const T v : vect)
        os << v << " ";
    os << "]";
    return os;
}

#endif
