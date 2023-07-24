#include "set.h"
#include <functional>
#include <tuple>

// because std::get<0, Key, Value> on it's own is overloaded and doesn't realise it can be casted to take a tuple
template <typename T1, typename T2>
std::function<T1(std::tuple<T1, T2>)> get_first_elem_func{
    [](const std::tuple<T1, T2> &tuple)
    { return std::get<0, T1, T2>(tuple); }};

template <typename Key, typename Value>
class Map
{
public:
    // typedef Item std::tuple<Key, Value>;

    constexpr Map(const size_t &size = 100000)
        : map_set{Set<Key, std::tuple<Key, Value>>(get_first_elem_func<Key, Value>, size)} {};

    // set item with that key to the given item, updating it if the key already exists
    void set(const Key &key, const Value &val)
    {
        map_set.set(key, std::make_tuple(key, val));
    }

    std::optional<Value> get(const Key &key) const
    {
        std::optional<std::tuple<Key, Value>> result{map_set.get(key)};
        if (result)
            return std::make_optional(std::get<1, Key, Value>(result.value()));
        return std::nullopt;
    }

    Value get(const Key &key, const Value &default_value) const
    {
        return get(key).value_or(default_value);
    }

    std::optional<Value> operator[](const Key &key) const { return get(key); }

    void update(const Map<Key, Value> &other)
    {
        for (const auto &[key, value] : other.items())
            set(key, value);
    }

    std::vector<std::tuple<Key, Value>> items() const { return map_set.items(); }

private:
    Set<Key, std::tuple<Key, Value>> map_set;
};

void test_tuple()
{
    std::tuple<int, bool> test_tuple{10, false};
    int result{std::get<0, int, bool>(test_tuple)};
    assert(result == 10);

    std::function<int(std::tuple<int, bool>)> test{
        [](std::tuple<int, bool> tuple_input)
        { return std::get<0, int, bool>(tuple_input); }};
    assert(test(std::make_tuple(10, true)) == 10);
    // Set<int, std::tuple<int, bool>> test_set(std::function<int(std::tuple<int, bool>)>{std::get<0, int, bool>});
    // test_set.add(std::make_tuple(10, true));
}

void test_map()
{
    Map<int, std::string> test_map{};
    test_map.set(0, "hello there!");
    test_map.set(1, "general kenobi!");
    assert(test_map.get(0) == "hello there!");
    assert(test_map.get(0) == "hello there!");
    assert(test_map.get(1) == "general kenobi!");
    assert(test_map.get(2) == std::nullopt);

    assert(test_map.get(0, "default") == "hello there!");
    assert(test_map.get(0, "default") == "hello there!");
    assert(test_map.get(1, "default") == "general kenobi!");
    assert(test_map.get(2, "default") == "default");

    assert(test_map[0] == "hello there!");
    assert(test_map[0] == "hello there!");
    assert(test_map[1] == "general kenobi!");
    assert(test_map[2] == std::nullopt);

    test_map.set(0, "your move");
    assert(test_map[0] == "your move");

    Map<int, std::string> to_update{};
    to_update.set(2, "you are a bold one!");
    to_update.update(test_map);
    assert(to_update[0] == "your move");
    assert(to_update[1] == "general kenobi!");
    assert(to_update[2] == "you are a bold one!");
}

int main()
{
    test_tuple();
    test_map();
}