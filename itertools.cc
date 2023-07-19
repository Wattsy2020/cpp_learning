#include <vector>
#include <iostream>
#include <tuple>
#include <exception>
#include "itertools.h"
#include "testlib.h"

void test_slicing()
{
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    assert(itertools::slice(test_vec, 0, 2) == (std::vector<int>{1, 2}));
    assert(itertools::slice(test_vec, 2, 3) == (std::vector<int>{3}));
    assert(itertools::slice(test_vec, 0, 0) == (std::vector<int>{}));
    assert(itertools::slice(test_vec, 0, -1) == (std::vector<int>{1, 2, 3, 4}));
    assert(itertools::slice(test_vec, 0, -2) == (std::vector<int>{1, 2, 3}));
    assert(itertools::slice(test_vec, -3, -1) == (std::vector<int>{3, 4}));
    assert(itertools::slice(test_vec, -1000, 1000) == test_vec);
}

void test_vector_outstream()
{
    testlib::assert_outstream(std::vector<int>{1, 2, 3, 4, 5}, "[ 1 2 3 4 5 ]");
}

void test_tuple_outstream()
{
    testlib::assert_outstream(std::make_tuple<int, int>(1, 2), "(1, 2)");
}

void test_optional_outstream()
{
    testlib::assert_outstream(std::optional<int>{}, "None");
    testlib::assert_outstream(std::optional<int>{2}, "2");
}

void test_reversed()
{
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    assert(itertools::reversed(test_vec) == (std::vector<int>{5, 4, 3, 2, 1}));
}

// tests uneven vectors as well
void test_zip()
{
    std::vector<int> test_vec{1, 2, 3};
    std::vector<int> equal_vec{5, 3, 1};
    std::vector<std::tuple<int, int>> expected_result{std::make_tuple(1, 5), std::make_tuple(2, 3), std::make_tuple(3, 1)};
    assert(itertools::zip(test_vec, equal_vec) == expected_result);

    std::vector<int> short_vec{itertools::slice(equal_vec, 0, 2)};
    assert(itertools::zip(test_vec, short_vec) == itertools::slice(expected_result, 0, 2));

    std::vector<int> long_vec{5, 3, 1, -1};
    assert(itertools::zip(test_vec, long_vec) == expected_result);
}

void test_range()
{
    assert(itertools::range(0, 5) == (std::vector<int>{0, 1, 2, 3, 4}));
    assert(itertools::range(2, 10) == (std::vector<int>{2, 3, 4, 5, 6, 7, 8, 9}));
    assert(itertools::range(2, 10, 2) == (std::vector<int>{2, 4, 6, 8}));
    assert(itertools::range(2, 10, 3) == (std::vector<int>{2, 5, 8}));
    assert(itertools::range(4, -4, -1) == (std::vector<int>{4, 3, 2, 1, 0, -1, -2, -3}));

    std::function<void()> invalid_range{[]()
                                        { itertools::range(1, 10, 0); }};
    testlib::raises<std::invalid_argument>(invalid_range);
}

void test_pairwise()
{
    std::vector<int> test_vec{1, 2, 3};
    std::vector<std::tuple<int, int>> expected_result{std::make_tuple(1, 2), std::make_tuple(2, 3)};
    assert(itertools::pairwise(test_vec) == expected_result);
}

void test_enumerate()
{
    std::vector<std::string> test_vec{"hello", "world", "!"};
    std::vector<std::tuple<int, std::string>> expected_result{std::make_tuple(0, "hello"), std::make_tuple(1, "world"), std::make_tuple(2, "!")};
    assert(itertools::enumerate(test_vec) == expected_result);
}

void test_init_last()
{
    std::vector<int> test_vec{1, 2, 3};
    std::tuple<std::vector<int>, int> result{itertools::init_last(test_vec)};
    assert(result == std::make_tuple(std::vector<int>{1, 2}, 3));

    std::function<void()> call_with_empty_vector{[]()
                                                 { itertools::init_last(std::vector<int>{}); }};
    testlib::raises<std::length_error>(call_with_empty_vector);
}

void test_join()
{
    std::vector<int> test_vec{1, 2, 3};
    std::string result{itertools::join(test_vec, " ")};
    assert(result == "1 2 3");
    assert(itertools::join(std::vector<int>{}, " ") == "");
}

void test_chain()
{
    std::vector<int> vec1{1, 2};
    std::vector<int> vec2{3, 4, 5};
    std::vector<int> combined{itertools::chain(vec1, vec2)};
    vec1[0] = 5; // ensure we're copying, not referencing, the input vectors
    assert(combined == (std::vector<int>{1, 2, 3, 4, 5}));
}

int main()
{
    test_slicing();
    test_vector_outstream();
    test_tuple_outstream();
    test_optional_outstream();
    test_reversed();
    test_zip();
    test_range();
    test_pairwise();
    test_enumerate();
    test_init_last();
    test_join();
    test_chain();

    // test if the templating works for strings as well
    std::vector<int> int_vec{1, 2, 3, 4, 5};
    std::vector<std::string> str_vec{"this", "is", "a", "string", "vector", "cool"};
    auto sliced{itertools::slice(str_vec, -4, -1)};
    std::cout << sliced << std::endl;
    std::cout << itertools::zip(sliced, int_vec) << std::endl;
    std::cout << itertools::pairwise(str_vec) << std::endl;
    std::cout << itertools::enumerate(str_vec) << std::endl;
    std::cout << itertools::pairwise(itertools::zip(sliced, int_vec)) << std::endl;
    std::cout << itertools::join(str_vec, " ") << std::endl;
}