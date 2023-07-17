#include <vector>
#include <optional>
#include <iostream>
#include <sstream>
#include <functional>
#include "itertools.h"

template <typename T>
class Set
{
public:
    Set<T>(size_t size = 100000)
        : hasher{std::hash<T>()},
          set_values{std::vector<std::optional<T>>(size)},
          all_items{std::vector<T>{}} {};

    void add(const T &item)
    {
        set_values[hash(item)] = std::optional<T>{item};
        all_items.push_back(item);
    }

    bool contains(const T &item) const
    {
        std::optional<T> set_value{set_values[hash(item)]};
        if (!set_value)
            return false;
        return set_value.value() == item;
    }

    std::vector<T> items() const { return all_items; }
    // TODO: overload constructor to take a vector

private:
    std::hash<T> hasher;
    std::vector<std::optional<T>> set_values;
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
    Set<int> set{};
    std::stringstream stream{};
    stream << set;
    assert(std::string(std::istreambuf_iterator<char>(stream), {}) == "{ }");

    set.add(1);
    set.add(1004);
    stream << set;
    assert(std::string(std::istreambuf_iterator<char>(stream), {}) == "{ 1 1004 }");
}

int main()
{
    test_set_contains();
    test_set_items();
    test_set_outstream();
}