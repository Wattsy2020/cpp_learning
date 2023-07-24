#include <assert.h>
#include <vector>
#include <iostream>
#include <functional>
#include <ranges>
#include <iterator>
#include "itertools.h"
#include "functools.h"

#ifndef LIAM_SET
#define LIAM_SET

template <typename HashType, typename ValueType = HashType>
class Set
{
public:
    constexpr Set(
        const std::function<HashType(ValueType)> key_func = std::identity(),
        const size_t &size = 100000)
        : hasher{std::hash<HashType>()},
          key_func{key_func},
          set_values{std::vector<std::vector<ValueType>>(size)},
          all_items{std::vector<ValueType>{}} {};

    template <std::ranges::input_range Iter>
        requires std::same_as<std::ranges::range_value_t<Iter>, ValueType>
    constexpr Set(
        const Iter &items,
        const std::function<HashType(ValueType)> key_func = std::identity(),
        size_t const &size = 100000) : Set(key_func, size)
    {
        add(items.cbegin(), items.cend());
    }

    constexpr Set(
        const std::initializer_list<ValueType> &items,
        const std::function<HashType(ValueType)> key_func = std::identity(),
        size_t const &size = 100000) : Set(key_func, size)
    {
        add(items.begin(), items.end());
    };

    template <std::input_iterator Iter>
        requires std::same_as<typename std::iterator_traits<Iter>::value_type, ValueType>
    constexpr Set(
        const Iter &begin,
        const Iter &end,
        const std::function<HashType(ValueType)> key_func = std::identity(),
        size_t const &size = 100000) : Set(key_func, size)
    {
        add(begin, end);
    }

    int size() const
    {
        return all_items.size();
    }

    // note mutable T shouldn't be hashed, so item should be immutable, and we can add a reference here
    void add(const ValueType &item)
    {
        if (contains(item))
            return;
        set_values[hash(item)].push_back(item);
        all_items.push_back(item);
    }

    template <std::input_iterator Iter>
        requires std::same_as<typename std::iterator_traits<Iter>::value_type, ValueType>
    void add(Iter begin, const Iter end)
    {
        for (; begin != end; ++begin)
            add(*begin);
    }

    // note this is O(n), if removing multiple items then use set::difference instead, which is also O(n)
    void remove(const ValueType &item)
    {
        if (!contains(item))
            return;
        std::erase(set_values[hash(item)], item);
        std::erase(all_items, item);
    }

    bool contains(const ValueType &item) const
    {
        std::function<bool(ValueType)> is_item{[item](ValueType value)
                                               { return value == item; }};
        return functools::any(is_item, set_values[hash(item)]);
    }

    std::vector<ValueType> items() const { return all_items; }

    // TODO: make these const_iterators, not pointers
    // note: must return constant pointers, to prevent returned items from being mutated,
    // and causing inconsistency between set_values and all_items
    const ValueType *begin() const { return &all_items[0]; }

    const ValueType *end() const { return &all_items[size()]; }

    operator bool() const { return !all_items.empty(); }

private:
    const std::hash<HashType> hasher;
    const std::function<HashType(ValueType)> key_func;
    std::vector<std::vector<ValueType>> set_values; // vector where vector[hash] is a vector containing all elemetns with that hash
    std::vector<ValueType> all_items;
    size_t hash(const ValueType &item) const
    {
        return hasher(key_func(item)) % set_values.size();
    };
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Set<T> &set)
{
    os << "{ ";
    for (const T &item : set.items())
        os << item << " ";
    os << "}";
    return os;
}

namespace set
{
    // given a set, returns a function that takes an item and checks if it belongs to that set
    template <typename T>
    constexpr std::function<bool(T)> get_overlap_func(const Set<T> &set)
    {
        return std::bind(&Set<T>::contains, set, std::placeholders::_1);
    }

    template <typename T>
    constexpr Set<T> set_union(const Set<T> &set1, const Set<T> &set2)
    {
        return Set<T>(itertools::chain(set1.items(), set2.items()));
    }

    template <typename T>
    constexpr Set<T> _intersection(const std::vector<T> &smaller_items, const Set<T> &larger_set)
    {
        return Set<T>(functools::filter(get_overlap_func(larger_set), smaller_items));
    }

    template <typename T>
    constexpr Set<T> intersection(const Set<T> &set1, const Set<T> &set2)
    {
        // Take the intersection by checking which items in the smaller set are also in the larger one
        std::vector<T> items1 = set1.items();
        std::vector<T> items2 = set2.items();
        if (items1.size() < items2.size())
            return _intersection(items1, set2);
        return _intersection(items2, set1);
    }

    template <typename T>
    constexpr Set<T> difference(const Set<T> &set_left, const Set<T> &set_right)
    {
        std::function<bool(T)> no_overlap{[set_right](const T &item)
                                          { return !set_right.contains(item); }};
        return Set<T>(functools::filter(no_overlap, set_left.items()));
    }

    template <typename T>
    constexpr bool is_subset(const Set<T> &set_left, const Set<T> &set_right)
    {
        return functools::all(get_overlap_func(set_right), set_left.items());
    }

    template <typename T>
    constexpr bool is_superset(const Set<T> &set_left, const Set<T> &set_right)
    {
        return is_subset(set_right, set_left);
    }
}

#endif
