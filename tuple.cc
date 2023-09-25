#include <concepts>
#include <memory>
#include <optional>
#include <stdexcept>
#include "ctest.h"

template <size_t Idx, typename Type, typename... Types>
struct _type_at_index
{
    using type = _type_at_index<Idx - 1, Types...>::type;
};

template <typename Type, typename... Types>
struct _type_at_index<0, Type, Types...>
{
    using type = Type;
};

template <size_t Idx, typename Type, typename... Types>
using type_at_index = _type_at_index<Idx, Type, Types...>::type;

// TODO: is there a way to have this stack allocated (e.g. using std::optional<Tuple<Types...>> next_tuple)?
// We'd need to indicate the size of an empty Tuple<> at compile time somehow
template <typename Type = std::nullopt_t, typename... Types>
class Tuple
{
public:
    Tuple(const Type &value, const Types &...remaining) : value{value}, next_tuple{make_next_tuple_ptr(remaining...)} {}

private:
    const Type value;
    const std::unique_ptr<Tuple<Types...>> next_tuple;

    constexpr std::unique_ptr<Tuple<Types...>> make_next_tuple_ptr(const Types &...items)
    {
        if constexpr (sizeof...(items) > 0)
            return std::make_unique<Tuple<Types...>>(items...);
        return nullptr;
    }
};

// implement constexpr tuple size (also using struct magic)

void test_get_type()
{
    static_assert(std::same_as<type_at_index<0, int>, int>);
    static_assert(std::same_as<type_at_index<0, int, bool, double>, int>);
    static_assert(std::same_as<type_at_index<1, int, bool, double>, bool>);
    static_assert(std::same_as<type_at_index<2, int, bool, double>, double>);
}

void test_tuple()
{
    Tuple<int> tuple1{1};
    Tuple<int, bool> tuple2(2, true);
    Tuple<int, bool, double> tuple3(2, true, 4.5);
}

int main()
{
    test_get_type();
    test_tuple();
}