#ifndef FUNCTOOLS
#define FUNCTOOLS

#include <vector>
#include <numeric>
#include <functional>

namespace functools
{
    // TODO: redo this in an iterator style like python, so we can lazily map
    template <typename T1, typename T2>
    constexpr std::vector<T2> map(const std::function<T2(T1)> &func, const std::vector<T1> &vec)
    {
        std::vector<T2> result{};
        for (const T1 &item : vec)
            result.push_back(func(item));
        return result;
    }

    template <typename T>
    constexpr std::vector<T> filter(const std::function<bool(T)> &pred, const std::vector<T> &vec)
    {
        std::vector<T> filtered{};
        for (const T &item : vec)
            if (pred(item))
                filtered.push_back(item);
        return filtered;
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
        for (const VAL &value : vec)
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

    // note: no type-safety for templates, unlike haskell I can see this takes any T
    // when really it needs a T that overloads (+)
    template <typename T>
    constexpr T sum(const std::vector<T> &vec)
    {
        return reduce(std::function<T(T, T)>{plus<T>}, vec, T{0});
        // return std::accumulate(vec.begin(), vec.end(), T{0}, std::plus<T>());
    }

    constexpr int bool_to_int(const bool &b) { return int(b); }

    template <typename T>
    constexpr int count(const std::function<bool(T)> &pred, const std::vector<T> &vec)
    {
        const std::function<int(T)> pred_to_int{compose(pred, std::function<int(bool)>{bool_to_int})};
        return sum(map(pred_to_int, vec));
    }
}

#endif
