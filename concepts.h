#ifndef LIAM_CONCEPTS
#define LIAM_CONCEPTS

#include <functional>
#include <concepts>

template <typename T>
concept Hashable = requires(T a) {
    {
        std::hash<T>{}(a)
    } -> std::convertible_to<std::size_t>;
};

#endif
