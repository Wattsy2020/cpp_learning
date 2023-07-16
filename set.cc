#include <vector>
#include <optional>
#include <iostream>
#include <sstream>
#include <functional>
#include "itertools.h"

class IntSet
{
public:
    IntSet(size_t size) : set_values{std::vector<std::optional<int>>(size)}, hasher{std::hash<int>()} {};
    void add(const int &item) { set_values[hash(item)] = std::optional<int>{item}; }
    bool contains(const int &item) const
    {
        std::optional<int> set_value{set_values[hash(item)]};
        if (!set_value)
            return false;
        return set_value.value() == item;
    }
    std::vector<int> items() const
    {
        std::vector<int> set_items{};
        for (const std::optional<int> &item : set_values)
            if (item)
                set_items.push_back(item.value());
        return set_items;
    }

    friend std::ostream &operator<<(std::ostream &os, const IntSet &set);
    // TODO: overload constructor and add to also take a vector
    // TODO: implement set arithematic operators, using operator overloading
    // TODO: make generic

private:
    std::hash<int> hasher;
    std::vector<std::optional<int>> set_values;
    size_t hash(const int &item) const
    {
        return hasher(item) % set_values.size();
    };
};

std::ostream &operator<<(std::ostream &os, const IntSet &set)
{
    os << "{ ";
    for (const int &item : set.items())
        os << item << " ";
    os << "}";
    return os;
}

void test_set_contains()
{
    IntSet set(100);
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
    IntSet set(100);
    assert(set.items() == std::vector<int>{});
    set.add(1);
    set.add(1004);
    assert(set.items() == (std::vector<int>{1, 1004}));
}

void test_set_outstream()
{
    IntSet set(100);
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