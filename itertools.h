#ifndef ITERTOOLS
#define ITERTOOLS

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <exception>
#include <optional>
#include <assert.h>
#include <ranges>
#include "math.h"
#include "strlib.h"

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::tuple<T1, T2> &tuple)
{
    auto [first, second] = tuple;
    os << "(" << first << ", " << second << ")";
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &optional)
{
    if (optional)
        os << optional.value();
    else
        os << "None";
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vect)
{
    os << "[ ";
    for (const T v : vect)
        os << v << " ";
    os << "]";
    return os;
}

namespace __private_utils
{
    // Keep the idx within the bounds of 0, and the vector's length (for slicing only)
    // if idx is out of bounds, will bind it to the minimum
    constexpr int _bound_index(const long &idx, const long &length)
    {
        const long result = (idx < 0) ? std::max(length + idx, long{0}) : std::min(idx, length);
        assert(result >= 0 && result <= length);
        return result;
    }
}

namespace itertools
{
    constexpr void validate_index(const int &index, const int &length)
    {
        if (index < 0 || index >= length)
            throw std::range_error(strlib::format("Invalid index {}, must be between 0 and length", index));
    }

    // Slice an iterator from [start, end)  (i.e. not including the end index)
    // Handles negative numbers and slices where end > length
    template <std::ranges::input_range Iter>
    constexpr Iter slice(const Iter &iter, const long &start, const long &end)
    {
        const long length{std::distance(iter.begin(), iter.end())};
        const long start_idx{__private_utils::_bound_index(start, length)};
        const long end_idx{__private_utils::_bound_index(end, length)};
        return Iter(iter.begin() + start_idx, iter.begin() + end_idx);
    }

    template <typename T>
    constexpr std::vector<T> reversed(const std::vector<T> &vec)
    {
        std::vector<T> output_vec(vec.size());
        std::reverse_copy(vec.begin(), vec.end(), output_vec.begin());
        return output_vec;
    }

    // A vector with items from [start, end) with the given step size
    constexpr std::vector<int> range(const int &start, const int &end, const int &step = 1)
    {
        if (step == 0)
        {
            throw std::invalid_argument("step cannot be 0");
        }
        else if (step > 0)
        {
            if (start > end)
                throw std::invalid_argument("need negative step when start > end");
        }
        else if (start < end)
        {
            throw std::invalid_argument("need positive step when start < end");
        }

        std::vector<int> result{};
        const int sign{signum(end - start)};
        for (int i{start}; sign * i < sign * end; i = i + step)
            result.push_back(i);
        return result;
    }

    // Zip two iterators together, returning a vector where vec[i] = tuple{iter1[i], iter2[i]}
    // Stop at the end of the shortest iterator
    // todo: use type aliases
    template <std::ranges::input_range Iter1, std::ranges::input_range Iter2>
    constexpr std::vector<std::tuple<typename Iter1::value_type, typename Iter2::value_type>> zip(const Iter1 &iter1, const Iter2 &iter2)
    {
        typename Iter1::const_iterator begin1{iter1.begin()};
        const typename Iter1::const_iterator end1{iter1.end()};
        typename Iter2::const_iterator begin2{iter2.begin()};
        const typename Iter2::const_iterator end2{iter2.end()};
        std::vector<std::tuple<typename Iter1::value_type, typename Iter2::value_type>> result{};
        while (begin1 != end1 && begin2 != end2)
        {
            result.push_back(std::make_tuple(*begin1, *begin2));
            ++begin1;
            ++begin2;
        }
        return result;
    }

    // Return tuple of i and the vector item at i
    template <typename T>
    constexpr std::vector<std::tuple<int, T>> enumerate(const std::vector<T> &vec)
    {
        return itertools::zip(itertools::range(0, vec.size()), vec);
    }

    // Return vector of tuples (item, next item)
    template <typename T>
    constexpr std::vector<std::tuple<T, T>> pairwise(const std::vector<T> &vec)
    {
        return itertools::zip(vec, itertools::slice(vec, 1, vec.size()));
    }

    // Return tuple (first part of vector, last element of vector)
    // Note the vector must contain at least one element
    template <typename T>
    constexpr std::tuple<std::vector<T>, T> init_last(const std::vector<T> &vec)
    {
        if (vec.empty())
            throw std::length_error("Vector must have at least one element");
        return std::make_tuple(itertools::slice(vec, 0, -1), vec[vec.size() - 1]);
    }

    // Return tuple (first element of vector, last part of vector)
    // Vector must contain at least one element
    template <typename T>
    constexpr std::tuple<std::vector<T>, T> head_tail(const std::vector<T> &vec)
    {
        if (vec.empty())
            throw std::length_error("Vector must have at least one element");
        return std::make_tuple(vec[0], itertools::slice(vec, 1, -1));
    }

    // Join elements of a vector together, using the separator
    template <typename T>
    std::string join(const std::vector<T> &vec, const std::string &separator)
    {
        if (vec.empty())
            return "";

        std::stringstream stream{};
        auto [init, last] = itertools::init_last(vec);
        for (const T &item : init)
            stream << item << separator;
        stream << last;
        return strlib::to_str(std::move(stream));
    }

    // Chain two vectors together
    // TODO: allow this function to take any number of vectors
    template <typename T>
    constexpr std::vector<T> chain(const std::vector<T> &vec1, const std::vector<T> &vec2)
    {
        std::vector<T> combined(vec1.size() + vec2.size());
        std::copy(vec1.begin(), vec1.end(), combined.begin());
        std::copy(vec2.begin(), vec2.end(), combined.begin() + vec1.size());
        return combined;
    }
}

#endif
