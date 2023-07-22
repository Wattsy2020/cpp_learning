#ifndef ITERTOOLS
#define ITERTOOLS

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <exception>
#include <optional>
#include "math.h"

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
    constexpr size_t _bound_index(const int &idx, const size_t &length)
    {
        if (idx < 0)
            return size_t(std::max(int(length) + idx, 0)); // cast to avoid overflow when length + idx < 0
        return std::min(size_t(idx), length);
    }
}

namespace itertools
{
    void validate_index(const int &index, const int &length)
    {
        if (index < 0 || index >= length)
            throw std::range_error("Invalid index, must be between 0 and length");
    }

    // Slice a vector from [start, end)  (i.e. not including the end index)
    // Handles negative numbers and slices where end > length
    template <typename T>
    constexpr std::vector<T> slice(const std::vector<T> &vec, const int &start, const int &end)
    {
        size_t length{vec.size()};
        size_t start_idx{__private_utils::_bound_index(start, length)};
        size_t end_idx{__private_utils::_bound_index(end, length)};
        return std::vector<T>(vec.begin() + start_idx, vec.begin() + end_idx);
    }

    template <typename T>
    constexpr std::vector<T> reversed(const std::vector<T> &vec)
    {
        std::vector<T> output_vec(vec.size());
        std::reverse_copy(vec.begin(), vec.end(), output_vec.begin());
        return output_vec;
    }

    // A vector with items from [start, end) with the given step size
    std::vector<int> range(const int &start, const int &end, const int &step = 1)
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
        int sign{signum(end - start)};
        for (int i{start}; sign * i < sign * end; i = i + step)
            result.push_back(i);
        return result;
    }

    // Merge two vectors together, returning a vector where vec[i] = tuple{vec1[i], vec2[i]}
    // Stop at the end of the shortest vector
    template <typename T1, typename T2>
    std::vector<std::tuple<T1, T2>> zip(const std::vector<T1> &vec1, const std::vector<T2> &vec2)
    {
        const size_t min_length{std::min(vec1.size(), vec2.size())};
        std::vector<std::tuple<T1, T2>> result{};
        for (size_t i{0}; i < min_length; i++)
            result.push_back(std::make_tuple(vec1[i], vec2[i]));
        return result;
    }

    // Return tuple of i and the vector item at i
    template <typename T>
    std::vector<std::tuple<int, T>> enumerate(const std::vector<T> &vec)
    {
        return itertools::zip(itertools::range(0, vec.size()), vec);
    }

    // Return vector of tuples (item, next item)
    template <typename T>
    std::vector<std::tuple<T, T>> pairwise(const std::vector<T> &vec)
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
        return std::string(std::istreambuf_iterator(stream), {});
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
