#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, std::tuple<T1, T2> tuple)
{
    auto [first, second] = tuple;
    os << "(" << first << ", " << second << ") ";
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, std::vector<T> vect)
{
    os << "[ ";
    for (const T v : vect)
        os << v << " ";
    os << "]";
    return os;
}

// Keep the idx within the bounds of 0, and the vector's length
size_t _bound_index(int idx, const size_t length)
{
    if (idx < 0)
        idx = length + idx;
    if (idx > length)
        idx = length;
    return idx;
}

// Slice a vector from [start, end)  (i.e. not including the end index)
// Handles negative numbers and slices where end > length
template <typename T>
std::vector<T> slice(const std::vector<T> &vec, int start, int end)
{
    size_t length{vec.size()};
    size_t start_idx{_bound_index(start, length)};
    size_t end_idx{_bound_index(end, length)};
    std::vector<T> sliced(vec.begin() + start_idx, vec.begin() + end_idx);
    return sliced;
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

// Return vector of tuples (item, next item)
template <typename T>
std::vector<std::tuple<T, T>> pairwise(const std::vector<T> &vec)
{
    return zip(vec, slice(vec, 1, vec.size()));
}

int main()
{
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    std::cout << slice(test_vec, 0, 2) << std::endl;
    std::cout << slice(test_vec, 2, 3) << std::endl;
    std::cout << slice(test_vec, 0, 0) << std::endl;
    std::cout << slice(test_vec, 0, -1) << std::endl;
    std::cout << slice(test_vec, 0, -2) << std::endl;
    std::cout << slice(test_vec, -3, -1) << std::endl;
    std::cout << test_vec << std::endl;

    std::vector<int> reversed(test_vec.size());
    std::reverse_copy(test_vec.begin(), test_vec.end(), reversed.begin());
    std::cout << reversed << std::endl;
    std::cout << std::make_tuple(1, 2) << std::endl;
    std::cout << zip(test_vec, reversed) << std::endl;

    // test the power of generics
    std::vector<std::string> str_vec{"this", "is", "a", "string", "vector", "cool"};
    auto sliced{slice(str_vec, -4, -1)};
    std::cout << sliced << std::endl;
    std::cout << zip(sliced, test_vec) << std::endl;

    std::cout << pairwise(test_vec) << std::endl;
}