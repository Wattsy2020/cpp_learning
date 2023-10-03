#ifndef LIAM_SET
#define LIAM_SET

#include <assert.h>
#include <vector>
#include <iostream>
#include <functional>
#include <ranges>
#include <iterator>
#include <optional>
#include <concepts>
#include "doubly_linked_list.h"
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
    typedef DoubleNode<ValueType> Node;
    typedef std::vector<std::shared_ptr<Node>> CacheSet;

    constexpr Set(
        const std::function<HashType(ValueType)> key_func = std::identity(),
        const size_t &size = set::HASHSET_INITIAL_SIZE)
        : hasher{std::hash<HashType>()},
          key_func{key_func},
          set_values{std::vector<CacheSet>(size)},
          linked_list{},
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
        return linked_list.size();
    }

    size_t capacity() const
    {
        return vec_capacity;
    }

    bool contains(const ValueType &item) const
    {
        return bool(find_node(item));
    }

    // note mutable T shouldn't be hashed, so item should be immutable, and we can add a reference here
    void add(const ValueType &item)
    {
        CacheSet &cache_set{set_values[hash(item)]};
        if (find_node(item, cache_set))
            return;
        _add(cache_set, item);
    }

    // set item with that key to the given item, updating it if the key already exists
    void set(const HashType &key, const ValueType &to_insert)
    {
        CacheSet &cache_set{set_values[hash_key(key)]};
        std::shared_ptr<Node> to_remove{find_node(key, cache_set)};
        _remove(cache_set, to_remove);
        _add(cache_set, to_insert);
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
        CacheSet &cache_set{set_values[hash(item)]};
        std::shared_ptr<Node> to_remove{find_node(item, cache_set)};
        _remove(cache_set, to_remove);
    }

    // get the full value of the item with this key
    std::optional<ValueType> get(const HashType &key) const
    {
        const CacheSet &cache_set{set_values[hash_key(key)]};
        std::shared_ptr<Node> found_ptr{find_node(key, cache_set)};
        if (found_ptr)
            return found_ptr->item;
        return std::nullopt;
    }

    std::vector<ValueType> items() const
    {
        return linked_list.items();
    }

    operator bool() const
    {
        return linked_list.size() > 0;
    }

private:
    const std::hash<HashType> hasher;
    const std::function<HashType(ValueType)> key_func;

    // vector where vector[hash] is a vector containing all elements with that hash
    size_t vec_capacity;
    std::vector<CacheSet> set_values;
    LinkedList<ValueType> linked_list;

    // find the node storing this item
    std::shared_ptr<Node> find_node(const ValueType &item) const
    {
        return find_node(item, set_values[hash(item)]);
    }

    std::shared_ptr<Node> find_node(const ValueType &item, const CacheSet &cache_set) const
    {
        auto opt_shared_ptr{functools::find([item](const std::shared_ptr<Node> node)
                                            { return node->item == item; },
                                            cache_set)};
        if (opt_shared_ptr)
            return opt_shared_ptr.value();
        return nullptr;
    }

    std::shared_ptr<Node> find_node(const HashType &key, const CacheSet &cache_set) const
        // only call this func when HashType is different
        // this requires clause ensures we don't have overlapping overloaded functions
        requires(!std::same_as<ValueType, HashType>)
    {
        auto opt_shared_ptr{
            functools::find([key, this](const std::shared_ptr<Node> node)
                            { return key_func(node->item) == key; },
                            cache_set)};
        if (opt_shared_ptr)
            return opt_shared_ptr.value();
        return nullptr;
    }

    void _add(CacheSet &cache_set, const ValueType &item)
    {
        std::shared_ptr<Node> node_ptr{linked_list.add_and_track(item)};
        cache_set.push_back(node_ptr);
        if (size() == vec_capacity)
            expand_capacity();
    }

    void _remove(CacheSet &cache_set, const std::shared_ptr<Node> to_remove)
    {
        if (!to_remove)
            return;
        std::erase(cache_set, to_remove);
        linked_list.remove(to_remove);
    }

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
