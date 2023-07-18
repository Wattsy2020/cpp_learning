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
    Set<int> set(range(1, 1000), 10);
    for (const int &i : range(1, 1000))
        assert(set.contains(i));
}

int main()
{
    test_set_contains();
    test_set_items();
    test_set_outstream();
    test_item_constructor();
    test_vector_constructor();
    test_hash_conflict();
}