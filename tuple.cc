#include <assert.h>
#include <concepts>
#include <optional>

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

void test_get_type()
{
    static_assert(std::same_as<type_at_index<0, int>, int>);
    static_assert(std::same_as<type_at_index<0, int, bool, double>, int>);
    static_assert(std::same_as<type_at_index<1, int, bool, double>, bool>);
    static_assert(std::same_as<type_at_index<2, int, bool, double>, double>);
}

int main()
{
    test_get_type();
}