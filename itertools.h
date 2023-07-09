#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <assert.h>
#include "math.h"

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::tuple<T1, T2> &tuple)
{
    auto [first, second] = tuple;
    os << "(" << first << ", " << second << ") ";
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

// Keep the idx within the bounds of 0, and the vector's length
constexpr size_t _bound_index(const int idx, const size_t length)
{
    assert(idx - length >= 0); // is out of range
    if (idx < 0)
        return length + idx;
    return std::min(size_t(idx), length);
}

// Slice a vector from [start, end)  (i.e. not including the end index)
// Handles negative numbers and slices where end > length
template <typename T>
constexpr std::vector<T> slice(const std::vector<T> &vec, const int start, const int end)
{
    size_t length{vec.size()};
    size_t start_idx{_bound_index(start, length)};
    size_t end_idx{_bound_index(end, length)};
    return std::vector<T>(vec.begin() + start_idx, vec.begin() + end_idx);
}

std::vector<int> range(int start, int end, int step = 1)
{
    if (start > end)
        assert(step < 0);

    std::vector<int> result;
    int sign = signum(end - start);
    for (int i = start; sign * i < sign * end; i = i + step)
        result.push_back(i);
    return result;
}

// Merge two vectors together, returning a vector where vec[i] = tuple{vec1[i], vec2[i]}
// Stop at the end of the shortest vector
template <typename T1, typename T2>
std::vector<std::tuple<T1, T2>> zip(const std::vector<T1> &vec1, const std::vector<T2> &vec2)
{
    const size_t min_length = std::min(vec1.size(), vec2.size());
    std::vector<std::tuple<T1, T2>> result;
    for (size_t i = 0; i < min_length; i++)
        result.push_back(std::make_tuple(vec1[i], vec2[i]));
    return result;
}

// Return tuple of i and the vector item at i
template <typename T>
std::vector<std::tuple<int, T>> enumerate(const std::vector<T> &vec)
{
    return zip(range(0, vec.size()), vec);
}

// Return vector of tuples (item, next item)
template <typename T>
std::vector<std::tuple<T, T>> pairwise(const std::vector<T> &vec)
{
    return zip(vec, slice(vec, 1, vec.size()));
}
