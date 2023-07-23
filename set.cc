#include <assert.h>
#include <vector>
#include <iostream>
#include <functional>
#include <ranges>
#include "itertools.h"
#include "functools.h"
#include "testlib.h"

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
        add(items);
    }

    constexpr Set(
        const std::initializer_list<ValueType> &items,
        const std::function<HashType(ValueType)> key_func = std::identity(),
        size_t const &size = 100000) : Set(key_func, size)
    {
        add(items);
    };

    int size() const { return all_items.size(); }

    // note mutable T shouldn't be hashed, so item should be immutable, and we can add a reference here
    void add(const ValueType &item)
    {
        if (contains(item))
            return;
        set_values[hash(item)].push_back(item);
        all_items.push_back(item);
    }

    template <std::ranges::input_range Iter>
        requires std::same_as<std::ranges::range_value_t<Iter>, ValueType>
    void add(const Iter &items)
    {
        auto first = items.begin();
        auto last = items.end();
        for (; first != last; ++first)
            add(*first);
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

    // TODO: make these iterators, not pointers
    // note: must return constant pointers, to prevent returned items from being mutated,
    // and causing inconsistency between set_values and all_items
    const ValueType *begin() const { return &all_items[0]; }

    const ValueType *end() const { return &all_items[size()]; }

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

void test_set_add()
{
    Set<int> set{};
    set.add(1);
    assert(set.contains(1));

    // test duplicates aren't added twice
    set.add(1);
    assert(set.contains(1));
    assert(set.items() == std::vector<int>{1});
}

void test_set_remove()
{
    Set<int> set{1, 2, 100001};
    set.remove(1);
    assert(set.items() == (std::vector<int>{2, 100001}));
    set.remove(3); // ensure no error is thrown
}

void test_set_contains()
{
    Set<int> set{};
    assert(!set.contains(1));
    assert(!set.contains(1004));
    set.add(1);
    set.add(1004);
    assert(set.contains(1));
    assert(set.contains(1004));
    assert(!set.contains(5));
}

void test_set_items()
{
    Set<int> set{};
    assert(set.items() == std::vector<int>{});
    set.add(1);
    set.add(1004);
    assert(set.items() == (std::vector<int>{1, 1004}));
}

void test_set_iteration()
{
    Set<int> set{1};
    for (int item : set)
        assert(item == 1);

    Set<int> to_sum{1, 2, 3, 4};
    assert(std::accumulate(to_sum.begin(), to_sum.end(), 0) == 10);

    // below works when returning T*, fails for const T*
    // trying to define T *begin() const fails, as the compiler recognizes returning a non const T* means the function cannot be const
    //*set.begin() = 5;
    // assert(set.items() == (std::vector<int>{5}));
}

void test_set_outstream()
{
    testlib::assert_outstream(Set<int>{}, "{ }");
    testlib::assert_outstream(Set<int>{1, 1004}, "{ 1 1004 }");
}

void test_vector_constructor()
{
    std::vector<int> input_vec{1, 10004};
    Set<int> set(input_vec);
    assert(set.items() == input_vec);
}

void test_initialiser_list_constructor()
{
    Set<int> set{1, 10004};
    assert(set.items() == (std::vector<int>{1, 10004}));
}

void test_hash_conflict()
{
    // using a small set size would cause hash conflicts, test if the set class still stores all the items
    std::vector<int> long_vector{itertools::range(1, 1000)};
    Set<int> set(long_vector, std::identity(), 10);
    for (const int &i : long_vector)
        assert(set.contains(i));
}

void test_set_union()
{
    Set<int> set1{1, 2, 3};
    Set<int> set2{2, 3, 4};
    Set<int> unioned_set{set::set_union(set1, set2)};
    assert(unioned_set.items() == (std::vector<int>{1, 2, 3, 4}));
}

void test_set_intersection()
{
    Set<int> set1{-1, 0, 1, 2, 3};
    Set<int> set2{2, 3, 4, 5};
    Set<int> result{set::intersection(set1, set2)};
    assert(result.items() == (std::vector<int>{2, 3}));
}

void test_set_difference()
{
    Set<int> set1{-1, 0, 1, 2, 3};
    Set<int> set2{2, 3, 4, 5};

    Set<int> diff1{set::difference(set1, set2)};
    assert(diff1.items() == (std::vector<int>{-1, 0, 1}));

    Set<int> diff2{set::difference(set2, set1)};
    assert(diff2.items() == (std::vector<int>{4, 5}));
}

void test_set_is_subset()
{
    Set<int> set1{2, 3};
    Set<int> set2{2, 3, 4, 5};
    assert(set::is_subset(set1, set2));
    assert(!set::is_subset(set2, set1));
}

void test_set_is_superset()
{
    Set<int> set1{2, 3};
    Set<int> set2{2, 3, 4, 5};
    assert(!set::is_superset(set1, set2));
    assert(set::is_superset(set2, set1));
}

void test_set_key_func()
{
    // add unhashable type, but with a function that converts it to a hashable type
    std::function<int(std::vector<int>)> key_func{[](const std::vector<int> &vec)
                                                  { return vec.back(); }};
    Set<int, std::vector<int>> set(key_func);
    std::vector<int> vec1{1, 2, 5};
    std::vector<int> vec2{1, 2};
    set.add(vec1);
    set.add(vec2);
    assert(set.contains(vec1));
    assert(set.contains(vec2));
    assert(!set.contains(std::vector<int>{1}));
    assert(set.size() == 2);
    assert(set.items() == (std::vector{vec1, vec2}));
}

int main()
{
    test_set_add();
    test_set_remove();
    test_set_contains();
    test_set_items();
    test_set_outstream();
    test_set_iteration();
    test_vector_constructor();
    test_hash_conflict();
    test_set_union();
    test_set_intersection();
    test_set_difference();
    test_set_is_subset();
    test_set_is_superset();
    test_set_key_func();
}