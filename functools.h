#ifndef FUNCTOOLS
#define FUNCTOOLS

#include <vector>
#include <numeric>
#include <functional>
#include <concepts>
#include <ranges>

namespace functools
{
    // TODO: redo this in an iterator style like python, so we can lazily map
    template <typename Arg, typename Func>
        requires std::regular_invocable<Func, Arg>
    constexpr std::vector<std::invoke_result_t<Func, Arg>> map(const Func &func, const std::vector<Arg> &vec)
    {
        std::vector<std::invoke_result_t<Func, Arg>> result{};
        for (const Arg &item : vec)
            result.push_back(func(item));
        return result;
    }

    template <typename Arg, typename Pred>
        requires std::predicate<Pred, Arg>
    constexpr std::vector<Arg> filter(const Pred &pred, const std::vector<Arg> &vec)
    {
        std::vector<Arg> filtered{};
        for (const Arg &item : vec)
            if (pred(item))
                filtered.push_back(item);
        return filtered;
    }

    // note: need to specify the input type to Func1, as Func1 could be overloaded and have multiple inputs
    template <typename Arg, typename Func1, typename Func2>
        requires std::regular_invocable<Func1, Arg> && std::regular_invocable<Func2, std::invoke_result_t<Func1, Arg>>
    constexpr std::function<std::invoke_result_t<Func2, std::invoke_result_t<Func1, Arg>>(Arg)>
    compose(const Func1 &func1, const Func2 &func2)
    {
        return {[&func1, &func2](Arg input_arg)
                { return func2(func1(input_arg)); }};
    }

    template <typename ACC, typename VAL, typename Func>
        requires std::regular_invocable<Func, VAL, ACC>
    constexpr ACC reduce(const Func &reducer, const std::vector<VAL> &vec, const ACC &init)
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

    template <typename T, typename Pred>
        requires std::predicate<Pred, T>
    constexpr bool any(const Pred &predicate, const std::vector<T> &vec)
    {
        return any(map(predicate, vec));
    }

    template <typename T, typename Pred>
        requires std::predicate<Pred, T>
    constexpr bool all(const Pred &predicate, const std::vector<T> &vec)
    {
        return all(map(predicate, vec));
    }

    template <typename T>
    constexpr T sum(const std::vector<T> &vec)
    {
        return std::accumulate(vec.begin(), vec.end(), T{0}, std::plus<T>());
    }

    constexpr int bool_to_int(const bool &b) { return int(b); }

    template <typename T, typename Pred>
        requires std::predicate<Pred, T>
    constexpr int count(const Pred &pred, const std::vector<T> &vec)
    {
        return sum(map(compose<T>(pred, bool_to_int), vec));
    }

    // Apply function to a value in an optional, returning a new optional with the resulting value, or an empty optional if there was to value
    template <typename T, typename Func>
        requires std::regular_invocable<Func, T>
    constexpr std::optional<std::invoke_result_t<Func, T>> transform(const Func &func, const std::optional<T> &item)
    {
        return (item) ? std::make_optional(func(item.value())) : std::nullopt;
    }

    // Find first item matching the predicate, if there is one
    template <typename T, typename Pred>
        requires std::predicate<Pred, T>
    constexpr std::optional<T> find(const Pred &pred, const std::vector<T> &vec)
    {
        for (const T &item : vec)
            if (pred(item))
                return std::make_optional(item);
        return std::nullopt;
    }

    // Apply a function to all items in an iterator
    // doesn't return the result, this is suited for functions that mutate items
    template <std::ranges::range Iter, typename Func>
        requires std::regular_invocable<Func, std::iter_value_t<Iter> &>
    constexpr void for_each(const Func &func, Iter &items)
    {
        for (std::iter_value_t<Iter> &item : items)
            func(item);
    }

    // TODO: make this take any number of type arguments
    template <typename ARG1, typename ARG2, typename RET>
    class Partial
    {
    public:
        constexpr Partial(const std::function<RET(const ARG1, const ARG2)> function, const ARG1 arg1) : function{function}, arg1{arg1} {}
        constexpr RET operator()(const ARG2 arg2) const { return function(arg1, arg2); }

    private:
        const std::function<RET(ARG1, ARG2)> function;
        const ARG1 arg1;
    };

    // TODO: could we directly define another constructor for std::function, so that it can implicitly convert Partial to std::function?
    template <typename ARG1, typename ARG2, typename RET>
    std::function<RET(ARG2)> to_func(const Partial<ARG1, ARG2, RET> &partial_func) { return std::function<RET(ARG2)>{partial_func}; }
}

#endif
