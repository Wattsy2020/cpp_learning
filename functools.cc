#include <vector>
#include <iostream>
#include <string>
#include <numeric>
#include "itertools.h"
#include "string_lib.h"

template <typename T1, typename T2>
constexpr std::vector<T2> map(const std::function<T2(T1)> &func, const std::vector<T1> &vec)
{
    std::vector<T2> result{};
    for (T1 item : vec)
        result.push_back(func(item));
    return result;
}

template <typename T1, typename T2, typename T3>
constexpr std::function<T3(T1)> compose(const std::function<T2(T1)> &func1, const std::function<T3(T2)> &func2)
{
    return std::function<T3(T1)>{
        [&func1, &func2](T1 input_arg)
        { return func2(func1(input_arg)); }};
}

template <typename ACC, typename VAL>
constexpr ACC reduce(const std::function<ACC(VAL, ACC)> &reducer, const std::vector<VAL> &vec, const ACC &init)
{
    ACC result{init};
    for (const VAL value : vec)
        result = reducer(result, value);
    return result;
}

// return true if any element in the vector is true
constexpr bool any(const std::vector<bool> &bools)
{
    for (const bool &value : bools)
        if (value)
            return true;
    return false;
}

// return true if all elements in the vector are true
constexpr bool all(const std::vector<bool> &bools)
{
    for (const bool &value : bools)
        if (!value)
            return false;
    return true;
}

// TODO: redo this in an iterator style like python, so we can lazily map. Is probably possible in C++
template <typename T>
constexpr bool any(const std::function<bool(T)> &predicate, const std::vector<T> &vec)
{
    return any(map(predicate, vec));
}

template <typename T>
constexpr bool all(const std::function<bool(T)> &predicate, const std::vector<T> &vec)
{
    return all(map(predicate, vec));
}

template <typename T>
constexpr T plus(const T &num1, const T &num2) { return num1 + num2; }

// TODO: implement our own reduce, and redo sum using it
// note: no type-safety for templates, unlike haskell I can see this takes any T
// when really it needs a T that overloads (+)
template <typename T>
constexpr T sum(const std::vector<T> &vec)
{
    return reduce(std::function<T(T, T)>{plus<T>}, vec, T{0});
    // return std::accumulate(vec.begin(), vec.end(), T{0}, std::plus<T>());
}

// TODO: probably can do this better
constexpr int bool_to_int(const bool &b) { return int(b); }

template <typename T>
constexpr int count(const std::function<bool(T)> &pred, const std::vector<T> &vec)
{
    const std::function<int(T)> pred_to_int{compose(pred, std::function<int(bool)>{bool_to_int})};
    return sum(map(pred_to_int, vec));
}

int main()
{
    std::vector<bool> bools{true, true, false, false};
    std::cout << to_str(any(bools)) << " " << to_str(all(bools)) << " for " << bools << std::endl;
    std::cout << to_str(all(std::vector<bool>{true, true, true})) << std::endl;

    std::vector<int> nums = range(1, 20, 2);
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::cout << nums << " " << map(greater_than_3, nums) << std::endl;
    std::cout << nums << " " << map(compose(greater_than_3, std::function<std::string(bool)>(to_str)), nums) << std::endl;
    std::cout << count(greater_than_3, nums) << std::endl;

    std::cout << sum(std::vector<int>{0, 1, 2, 3, 4}) << std::endl;
    std::cout << sum(std::vector<double>{-1.333, 1.333, 2.1}) << std::endl;

    std::cout << to_str(all(greater_than_3, nums)) << std::endl;
    std::cout << to_str(any(greater_than_3, nums)) << std::endl;
}