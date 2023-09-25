#include <concepts>
#include <memory>
#include <optional>
#include <stdexcept>
#include "ctest.h"

struct __end_of_args_sentinel
{
};

template <typename Type = __end_of_args_sentinel, typename... Types>
constexpr size_t num_args = 1 + num_args<Types...>;

template <typename... Types>
constexpr size_t num_args<__end_of_args_sentinel, Types...> = 0;

template <size_t Idx, typename Type, typename... Types>
struct _type_at_index
{
    static_assert(Idx <= sizeof...(Types));
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
// current problem: if next_tuple is optional, then it could potentially have infinite size
// as each next_tuple could have their own next_tuple, so it can’t be stack allocated
// We'd need to indicate the size of an empty Tuple<> at compile time somehow
template <typename Type = std::nullopt_t, typename... Types>
class Tuple
{
public:
    constexpr Tuple(const Type &value, const Types &...remaining) : value{value}, next_tuple{make_next_tuple_ptr(remaining...)} {}

    template <size_t Idx>
    constexpr type_at_index<Idx, Type, Types...> get()
    {
        // note: use "if constexpr" to avoid compiling get_tuple_elem<-1, <>> which is an invalid template
        if constexpr (Idx == 0)
            return value;
        else
            return next_tuple->template get<Idx - 1>();
    }

    friend bool operator==(const Tuple<Type, Types...> &left, const Tuple<Type, Types...> &right)
    {
        if (left.value != right.value)
            return false;
        if constexpr (sizeof...(Types) > 0)
            return *left.next_tuple == *right.next_tuple;
        return true;
    }

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

template <typename... Types>
constexpr int tuple_size(const Tuple<Types...> &tuple)
{
    return num_args<Types...>;
}

void test_get_type()
{
    static_assert(std::same_as<type_at_index<0, int>, int>);
    static_assert(std::same_as<type_at_index<0, int, bool, double>, int>);
    static_assert(std::same_as<type_at_index<1, int, bool, double>, bool>);
    static_assert(std::same_as<type_at_index<2, int, bool, double>, double>);
}

void test_args_size()
{
    static_assert(num_args<int> == 1);
    static_assert(num_args<int, bool> == 2);
    static_assert(num_args<int, bool, double> == 3);
}

void test_tuple()
{
    Tuple<int> tuple1{1};
    Tuple<int, bool> tuple2{2, true};
    Tuple<int, bool, double> tuple3{2, true, 4.5};

    ctest::assert_equal(tuple1.get<0>(), 1);
    ctest::assert_equal(tuple2.get<0>(), 2);
    ctest::assert_equal(tuple2.get<1>(), true);
    ctest::assert_equal(tuple3.get<0>(), 2);
    ctest::assert_equal(tuple3.get<1>(), true);
    ctest::assert_equal(tuple3.get<2>(), 4.5);

    static_assert(tuple_size(tuple1) == 1);
    static_assert(tuple_size(tuple2) == 2);
    static_assert(tuple_size(tuple3) == 3);

    ctest::assert_equal(tuple2, tuple2);
    ctest::assert_equal(tuple3, tuple3);
    assert(tuple3 != (Tuple<int, bool, double>{2, true, 4.3}));
}

int main()
{
    test_get_type();
    test_args_size();
    test_tuple();
}