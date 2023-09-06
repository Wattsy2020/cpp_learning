#ifndef LIAM_SET
#define LIAM_SET

#include <assert.h>
#include <vector>
#include <iostream>
#include <functional>
#include <ranges>
#include <iterator>
#include <optional>
#include "itertools.h"
#include "functools.h"
#include "concepts.h"

namespace set
{
    constexpr int HASHSET_INITIAL_SIZE{8};
}

template <Hashable HashType, typename ValueType = HashType>
class Set
{
public:
    typedef ValueType value_type;
    typedef const ValueType *const_iterator;

    constexpr Set(
        const std::function<HashType(ValueType)> key_func = std::identity(),
        const size_t &size = set::HASHSET_INITIAL_SIZE)
        : hasher{std::hash<HashType>()},
          key_func{key_func},
          set_values{std::vector<std::vector<ValueType>>(size)},
          num_items{0},
          vec_capacity{size} {};

    template <std::ranges::input_range Iter>
        requires std::same_as<std::ranges::range_value_t<Iter>, ValueType>
    constexpr Set(
        const Iter &items,
        const std::function<HashType(ValueType)> key_func = std::identity(),
        size_t const &size = set::HASHSET_INITIAL_SIZE) : Set(key_func, size)
    {
        add(items.begin(), items.end());
    }

    constexpr Set(
        std::initializer_list<ValueType> items,
        const std::function<HashType(ValueType)> key_func = std::identity(),
        size_t const &size = set::HASHSET_INITIAL_SIZE) : Set(key_func, size)
    {
        add(items.begin(), items.end());
    };

    size_t size() const
    {
        return num_items;
    }

    size_t capacity() const
    {
        return vec_capacity;
    }

    bool contains(const ValueType &item) const
    {
        return functools::any(
            [item](ValueType value)
            { return value == item; },
            set_values[hash(item)]);
    }

    // note mutable T shouldn't be hashed, so item should be immutable, and we can add a reference here
    void add(const ValueType &item)
    {
        if (contains(item))
            return;
        set_values[hash(item)].push_back(item);
        ++num_items;
        if (num_items == vec_capacity)
            expand_capacity();
    }

    // set item with that key to the given item, updating it if the key already exists
    void set(const HashType &key, const ValueType &to_insert)
    {
        std::optional<ValueType> existing_item{get(key)};
        if (existing_item)
            remove(existing_item.value());
        add(to_insert);
    }

    template <std::input_iterator Iter>
        requires std::same_as<typename std::iterator_traits<Iter>::value_type, ValueType>
    void add(Iter begin, const Iter end)
    {
        for (; begin != end; ++begin)
            add(*begin);
    }

    void remove(const ValueType &item)
    {
        if (!contains(item))
            return;
        std::erase(set_values[hash(item)], item);
        --num_items;
    }

    // get the full value of the item with this key
    std::optional<ValueType> get(const HashType &key) const
    {
        return functools::find([key, this](const ValueType &item)
                               { return key_func(item) == key; },
                               set_values[hash_key(key)]);
    }

    // retrieve all items in the set. this is an O(n) operation
    // insertion order is not preserved
    std::vector<ValueType> items() const
    {
        std::vector<ValueType> result_items{};
        for (const std::vector<ValueType> &hash_values : set_values)
            result_items.insert(result_items.end(), hash_values.begin(), hash_values.end());
        return result_items;
    }

    operator bool() const
    {
        return num_items > 0;
    }

private:
    const std::hash<HashType> hasher;
    const std::function<HashType(ValueType)> key_func;
    std::vector<std::vector<ValueType>> set_values; // vector where vector[hash] is a vector containing all elements with that hash
    size_t num_items;
    size_t vec_capacity;

    // doubles the vec_capacity of set_values, to reduce hash conflicts when the vec_capacity is reached
    void expand_capacity()
    {
        vec_capacity = vec_capacity * 2;
        std::vector<ValueType> all_items{items()};
        set_values.clear();
        set_values.resize(vec_capacity);
        add(all_items.begin(), all_items.end());
    }

    size_t hash_key(const HashType &item) const
    {
        return hasher(item) % vec_capacity;
    };

    size_t hash(const ValueType &item) const
    {
        return hash_key(key_func(item));
    }
};

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
        auto no_overlap{[set_right](const T &item)
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

template <typename T>
std::ostream &operator<<(std::ostream &os, const Set<T> &set)
{
    os << "{ ";
    for (const T &item : set.items())
        os << item << " ";
    os << "}";
    return os;
}

template <typename T>
bool operator==(const Set<T> &left, const Set<T> &right)
{
    return left.size() == right.size() && set::is_subset(left, right) && set::is_subset(right, left);
}

template <typename T>
bool operator!=(const Set<T> &left, const Set<T> &right) { return !(left == right); }

#endif
