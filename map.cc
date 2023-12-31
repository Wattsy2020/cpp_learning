#include <functional>
#include <tuple>
#include "set.h"
#include "concepts.h"
#include "functools.h"
#include "ctest.h"

// because std::get<0, Key, Value> on its own is overloaded and doesn't realise it can take a tuple
template <size_t ElementIdx, typename T1, typename T2>
std::function<std::tuple_element_t<ElementIdx, std::tuple<T1, T2>>(std::tuple<T1, T2>)> get_elem{
    [](const std::tuple<T1, T2> &tuple)
    { return std::get<ElementIdx, T1, T2>(tuple); }};

template <Hashable Key, typename Value>
class Map
{
public:
    typedef std::tuple<Key, Value> Item;

    constexpr Map(const size_t &size = set::HASHSET_INITIAL_SIZE)
        : map_set{Set<Key, Item>(get_elem<0, Key, Value>, size)} {};

    constexpr Map(std::initializer_list<Item> items, const size_t &size = set::HASHSET_INITIAL_SIZE) : Map(size)
    {
        for (const Item &item : items)
            set(item);
    }

    // set item with that key to the given item, updating it if the key already exists
    void set(const Key &key, const Value &val)
    {
        map_set.set(key, std::make_tuple(key, val));
    }

    void set(const Item &item)
    {
        map_set.set(std::get<0, Key, Value>(item), item);
    }

    std::optional<Value> get(const Key &key) const
    {
        return functools::transform(get_elem<1, Key, Value>, map_set.get(key));
    }

    Value get(const Key &key, const Value &default_value) const
    {
        return get(key).value_or(default_value);
    }

    std::optional<Value> operator[](const Key &key) const
    {
        return get(key);
    }

    void update(const Map<Key, Value> &other)
    {
        for (const Item &item : other.items())
            set(item);
    }

    std::vector<Item> items() const
    {
        return map_set.items();
    }

    size_t size() const
    {
        return map_set.size();
    }

    operator bool() const
    {
        return size() > 0;
    }

    friend bool operator==(const Map<Key, Value> &left, const Map<Key, Value> &right) { return left.map_set == right.map_set; }

    friend bool operator!=(const Map<Key, Value> &left, const Map<Key, Value> &right) { return !(left == right); }

private:
    Set<Key, Item> map_set;
};

void test_tuple()
{
    std::tuple<int, bool> test_tuple{10, false};
    int result{std::get<0, int, bool>(test_tuple)};
    ctest::assert_equal(result, 10);

    std::function<int(std::tuple<int, bool>)> test{
        [](std::tuple<int, bool> tuple_input)
        { return std::get<0, int, bool>(tuple_input); }};
    ctest::assert_equal(test(std::make_tuple(10, true)), 10);
}

void test_map()
{
    Map<int, std::string> test_map{};
    ctest::assert_equal(test_map.size(), 0);
    assert(!test_map);

    test_map.set(0, "hello there!");
    test_map.set(1, "general kenobi!");
    ctest::assert_equal(test_map.get(0), "hello there!");
    ctest::assert_equal(test_map.get(0), "hello there!");
    ctest::assert_equal(test_map.get(1), "general kenobi!");
    assert(!test_map.get(2));
    assert(test_map);
    ctest::assert_equal(test_map.size(), 2);

    ctest::assert_equal(test_map.get(0, "default"), "hello there!");
    ctest::assert_equal(test_map.get(0, "default"), "hello there!");
    ctest::assert_equal(test_map.get(1, "default"), "general kenobi!");
    ctest::assert_equal(test_map.get(2, "default"), "default");

    ctest::assert_equal(test_map[0], "hello there!");
    ctest::assert_equal(test_map[0], "hello there!");
    ctest::assert_equal(test_map[1], "general kenobi!");
    assert(!test_map[2]);

    test_map.set(0, "your move");
    ctest::assert_equal(test_map[0], "your move");
    std::vector<std::tuple<int, std::string>> expected_items{{1, "general kenobi!"}, {0, "your move"}};
    ctest::assert_equal(test_map.items(), expected_items);

    Map<int, std::string> to_update{};
    to_update.set(2, "you are a bold one!");
    to_update.update(test_map);
    ctest::assert_equal(to_update[0], "your move");
    ctest::assert_equal(to_update[1], "general kenobi!");
    ctest::assert_equal(to_update[2], "you are a bold one!");
    std::vector<std::tuple<int, std::string>> expected_items2{{2, "you are a bold one!"}, {1, "general kenobi!"}, {0, "your move"}};
    ctest::assert_equal(to_update.items(), expected_items2);
}

void test_map_initializer_list()
{
    Map<int, std::string> map1{{0, "hello"}, {1, "there"}};
    Map<int, std::string> map2{};
    map2.set(1, "there");
    map2.set(0, "hello");
    ctest::assert_equal(map1, map2);
}

int main()
{
    test_tuple();
    test_map();
    test_map_initializer_list();
}