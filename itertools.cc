#include <vector>
#include <iostream>
#include <tuple>
#include <sstream>
#include <exception>
#include "itertools.h"

void test_slicing()
{
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    assert(slice(test_vec, 0, 2) == (std::vector<int>{1, 2}));
    assert(slice(test_vec, 2, 3) == (std::vector<int>{3}));
    assert(slice(test_vec, 0, 0) == (std::vector<int>{}));
    assert(slice(test_vec, 0, -1) == (std::vector<int>{1, 2, 3, 4}));
    assert(slice(test_vec, 0, -2) == (std::vector<int>{1, 2, 3}));
    assert(slice(test_vec, -3, -1) == (std::vector<int>{3, 4}));
    assert(slice(test_vec, -1000, 1000) == test_vec);
}

void test_vector_outstream()
{
    std::stringstream stream{};
    stream << std::vector<int>{1, 2, 3, 4, 5};
    std::string result(std::istreambuf_iterator(stream), {});
    assert(result == "[ 1 2 3 4 5 ]");
}

void test_tuple_outstream()
{
    std::stringstream stream{};
    stream << std::make_tuple<int, int>(1, 2);
    std::string result(std::istreambuf_iterator(stream), {});
    assert(result == "(1, 2)");
}

void test_reversed()
{
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    assert(reversed(test_vec) == (std::vector<int>{5, 4, 3, 2, 1}));
}

// tests uneven vectors as well
void test_zip()
{
    std::vector<int> test_vec{1, 2, 3};
    std::vector<int> equal_vec{5, 3, 1};
    std::vector<std::tuple<int, int>> expected_result{std::make_tuple(1, 5), std::make_tuple(2, 3), std::make_tuple(3, 1)};
    assert(zip(test_vec, equal_vec) == expected_result);

    std::vector<int> short_vec{slice(equal_vec, 0, 2)};
    assert(zip(test_vec, short_vec) == slice(expected_result, 0, 2));

    std::vector<int> long_vec{5, 3, 1, -1};
    assert(zip(test_vec, long_vec) == expected_result);
}

void test_range()
{
    assert(range(0, 5) == (std::vector<int>{0, 1, 2, 3, 4}));
    assert(range(2, 10) == (std::vector<int>{2, 3, 4, 5, 6, 7, 8, 9}));
    assert(range(2, 10, 2) == (std::vector<int>{2, 4, 6, 8}));
    assert(range(2, 10, 3) == (std::vector<int>{2, 5, 8}));
    assert(range(4, -4, -1) == (std::vector<int>{4, 3, 2, 1, 0, -1, -2, -3}));

    try
    {
        range(1, 10, 0);
    }
    catch (std::invalid_argument)
    {
        return;
    }
    assert("test failed, did not raise exception");
}

void test_pairwise()
{
    std::vector<int> test_vec{1, 2, 3};
    std::vector<std::tuple<int, int>> expected_result{std::make_tuple(1, 2), std::make_tuple(2, 3)};
    assert(pairwise(test_vec) == expected_result);
}

void test_enumerate()
{
    std::vector<std::string> test_vec{"hello", "world", "!"};
    std::vector<std::tuple<int, std::string>> expected_result{std::make_tuple(0, "hello"), std::make_tuple(1, "world"), std::make_tuple(2, "!")};
    assert(enumerate(test_vec) == expected_result);
}

void test_init_last()
{
    std::vector<int> test_vec{1, 2, 3};
    std::tuple<std::vector<int>, int> result{init_last(test_vec)};
    assert(result == std::make_tuple(std::vector<int>{1, 2}, 3));

    try
    {
        init_last(std::vector<int>{});
    }
    catch (std::length_error)
    {
        return;
    }
    assert("test failed, did not raise exception");
}

void test_join()
{
    std::vector<int> test_vec{1, 2, 3};
    std::string result{join(test_vec, " ")};
    assert(result == "1 2 3");
}

int main()
{
    test_slicing();
    test_vector_outstream();
    test_tuple_outstream();
    test_reversed();
    test_zip();
    test_range();
    test_pairwise();
    test_enumerate();
    test_init_last();

    // test if the templating works for strings as well
    std::vector<int> int_vec{1, 2, 3, 4, 5};
    std::vector<std::string> str_vec{"this", "is", "a", "string", "vector", "cool"};
    auto sliced{slice(str_vec, -4, -1)};
    std::cout << sliced << std::endl;
    std::cout << zip(sliced, int_vec) << std::endl;
    std::cout << pairwise(str_vec) << std::endl;
    std::cout << enumerate(str_vec) << std::endl;
    std::cout << pairwise(zip(sliced, int_vec)) << std::endl;
    std::cout << join(str_vec, " ") << std::endl;
}