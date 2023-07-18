#include <vector>
#include <optional>
#include <iostream>
#include <sstream>
#include <functional>
#include "itertools.h"
#include "functools.h"
#include "testlib.h"

template <typename T>
class Set
{
public:
    constexpr Set<T>(const size_t &size = 100000)
        : hasher{std::hash<T>()},
          set_values{std::vector<std::vector<T>>(size)},
          all_items{std::vector<T>{}} {};

    constexpr Set<T>(const T &init_item, const size_t &size = 100000) : Set<T>(size)
    {
        add(init_item);
    };

    constexpr Set<T>(const std::vector<T> &items, size_t const &size = 100000) : Set<T>(size)
    {
        for (const T &item : items)
            add(item);
    };

    constexpr Set<T>(const std::initializer_list<T> &items, size_t const &size = 100000) : Set<T>(size)
    {
        for (const T &item : items)
            add(item);
    };

    // note mutable T shouldn't be hashed, so we can add a reference here
    void add(const T &item)
    {
        if (contains(item))
            return;
        set_values[hash(item)].push_back(item);
        all_items.push_back(item);
    }

    bool contains(const T &item) const
    {
        std::function<bool(T)> is_item{[item](T value)
                                       { return value == item; }};
        return any(is_item, set_values[hash(item)]);
    }

    std::vector<T> items() const { return all_items; }

private:
    std::hash<T> hasher;
    std::vector<std::vector<T>> set_values; // vector where vector[hash] is a vector containing all elemetns with that hash
    std::vector<T> all_items;
    size_t hash(const T &item) const
    {
        return hasher(item) % set_values.size();
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
    template <typename T>
    constexpr Set<T> set_union(const Set<T> &set1, const Set<T> &set2)
    {
        return Set<T>(itertools::chain(set1.items(), set2.items()));
    }

    template <typename T>
    constexpr Set<T> _intersection(const std::vector<T> &smaller_items, const Set<T> &larger_set)
    {
        std::function<bool(T)> overlaps = {[larger_set](const T &item)
                                           { return larger_set.contains(item); }};
        return Set<T>(filter(overlaps, smaller_items));
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
        return Set<T>(filter(no_overlap, set_left.items()));
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

void test_set_outstream()
{
    testlib::assert_outstream(Set<int>{}, "{ }");
    testlib::assert_outstream(Set<int>{1, 1004}, "{ 1 1004 }");
}

void test_item_constructor()
{
    Set<int> set(3);
    assert(set.contains(3));
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
    Set<int> set(long_vector, 10);
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

int main()
{
    test_set_add();
    test_set_contains();
    test_set_items();
    test_set_outstream();
    test_item_constructor();
    test_vector_constructor();
    test_hash_conflict();
    test_set_union();
    test_set_intersection();
    test_set_difference();
}