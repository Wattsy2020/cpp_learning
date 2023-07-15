#include <vector>
#include <optional>
#include <iostream>
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

int main()
{
    IntSet set{IntSet(100)};
    std::cout << set << std::endl;
    set.add(1);
    set.add(1004);
    std::cout << set.contains(1) << set.contains(10) << set.contains(1004) << std::endl;
    std::cout << set << std::endl;
}