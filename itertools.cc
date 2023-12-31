#include <assert.h>
#include <vector>
#include <list>
#include <forward_list>
#include <tuple>
#include <iostream>
#include <exception>
#include <chrono>
#include "itertools.h"
#include "ctest.h"

void test_slice()
{
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    ctest::assert_equal(itertools::to_vec(itertools::slice(test_vec, 0, 2)), std::vector<int>{1, 2});
    ctest::assert_equal(itertools::to_vec(itertools::slice(test_vec, 2, 3)), std::vector<int>{3});
    ctest::assert_equal(itertools::to_vec(itertools::slice(test_vec, 0, 0)), std::vector<int>{});
    ctest::assert_equal(itertools::to_vec(itertools::slice(test_vec, 0, -1)), std::vector<int>{1, 2, 3, 4});
    ctest::assert_equal(itertools::to_vec(itertools::slice(test_vec, 0, -2)), std::vector<int>{1, 2, 3});
    ctest::assert_equal(itertools::to_vec(itertools::slice(test_vec, -3, -1)), std::vector<int>{3, 4});
    ctest::assert_equal(itertools::to_vec(itertools::slice(test_vec, -1000, 1000)), test_vec);
}

void test_reversed()
{
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    ctest::assert_equal(itertools::reversed(test_vec), std::vector<int>{5, 4, 3, 2, 1});
}

// tests uneven vectors as well
void test_zip()
{
    std::vector<int> test_vec{1, 2, 3};
    std::vector<int> equal_vec{5, 3, 1};
    std::vector<std::tuple<int, int>> expected_result{std::make_tuple(1, 5), std::make_tuple(2, 3), std::make_tuple(3, 1)};
    ctest::assert_equal(itertools::zip(test_vec, equal_vec), expected_result);

    std::vector<int> short_vec{itertools::to_vec(itertools::slice(equal_vec, 0, 2))};
    ctest::assert_equal(itertools::zip(test_vec, short_vec), itertools::to_vec(itertools::slice(expected_result, 0, 2)));

    std::vector<int> long_vec{5, 3, 1, -1};
    ctest::assert_equal(itertools::zip(test_vec, long_vec), expected_result);
}

void test_range()
{
    ctest::assert_equal(itertools::range(0, 5), std::vector<int>{0, 1, 2, 3, 4});
    ctest::assert_equal(itertools::range(2, 10), std::vector<int>{2, 3, 4, 5, 6, 7, 8, 9});
    ctest::assert_equal(itertools::range(2, 10, 2), std::vector<int>{2, 4, 6, 8});
    ctest::assert_equal(itertools::range(2, 10, 3), std::vector<int>{2, 5, 8});
    ctest::assert_equal(itertools::range(4, -4, -1), std::vector<int>{4, 3, 2, 1, 0, -1, -2, -3});

    std::function<void()> invalid_range{[]()
                                        { itertools::range(1, 10, 0); }};
    ctest::raises<std::invalid_argument>(invalid_range);
}

void test_pairwise()
{
    std::vector<int> test_vec{1, 2, 3};
    std::vector<std::tuple<int, int>> expected_result{std::make_tuple(1, 2), std::make_tuple(2, 3)};
    ctest::assert_equal(itertools::pairwise(test_vec), expected_result);
}

void test_enumerate()
{
    std::vector<std::string> test_vec{"hello", "world", "!"};
    std::vector<std::tuple<int, std::string>> expected_result{std::make_tuple(0, "hello"), std::make_tuple(1, "world"), std::make_tuple(2, "!")};
    ctest::assert_equal(itertools::enumerate(test_vec), expected_result);
}

void test_init_last()
{
    std::vector<int> test_vec{1, 2, 3};
    std::tuple<std::vector<int>, int> result{itertools::init_last(test_vec)};
    ctest::assert_equal(result, std::make_tuple(std::vector<int>{1, 2}, 3));

    std::function<void()> call_with_empty_vector{[]()
                                                 { itertools::init_last(std::vector<int>{}); }};
    ctest::raises<std::length_error>(call_with_empty_vector);
}

void test_head_tail()
{
    std::vector<int> test_vec{1, 2, 3};
    std::tuple<int, std::vector<int>> result{itertools::head_tail(test_vec)};
    ctest::assert_equal(result, std::make_tuple(1, std::vector<int>{2, 3}));
}

void test_join()
{
    std::vector<int> test_vec{1, 2, 3};
    std::string result{itertools::join(test_vec, " ")};
    ctest::assert_equal(result, "1 2 3");
    ctest::assert_equal(itertools::join(std::vector<int>{}, " "), "");
}

void test_chain_class()
{
    static_assert(std::ranges::input_range<__itertools_utils::Chain<int>>);
    std::vector<int> vec1{1, 2};
    std::vector<int> vec2{3, 4, 5};
    __itertools_utils::Chain<int> chain(vec1, vec2);
    __itertools_utils::Chain<int>::ChainIterator iter{chain.begin()};
    ctest::assert_equal(*iter, 1);
    ctest::assert_equal(*++iter, 2);
    ctest::assert_equal(*++iter, 3);
    ctest::assert_equal(*++iter, 4);
    ctest::assert_equal(*++iter, 5);
    ++iter;
    __itertools_utils::Chain<int>::ChainIterator end{chain.end()};
    ctest::assert_equal(iter, end);
    std::vector<int> extracted(chain.begin(), chain.end());
    ctest::assert_equal(extracted, std::vector<int>{1, 2, 3, 4, 5});
}

void test_chain()
{
    std::vector<int> vec1{1, 2};
    std::vector<int> vec2{3, 4, 5};
    std::vector<int> combined(itertools::to_vec(itertools::chain(vec1, vec2)));
    vec1[0] = 5; // ensure we're copying, not referencing, the input vectors
    ctest::assert_equal(combined, std::vector<int>{1, 2, 3, 4, 5});

    std::list<int> list1{6, 7, 8};
    std::vector<int> combined2(itertools::to_vec(itertools::chain(combined, list1)));
    ctest::assert_equal(combined2, itertools::range(1, 9));

    std::vector<int> combined3(
        itertools::to_vec(
            itertools::chain(
                std::vector<int>{1, 2},
                std::forward_list<int>{3, 4},
                std::list<int>{5, 6},
                std::vector<int>{7, 8})));
    ctest::assert_equal(combined3, itertools::range(1, 9));

    // benchmark
    std::chrono::time_point start{std::chrono::system_clock::now()};
    std::vector<int> range1{itertools::range(1, 100000)};
    std::vector<int> range2{itertools::range(-100000, 0)};
    std::vector<int> range3{itertools::range(-100000, 100000)};
    for (const int i : itertools::range(1, 10))
        itertools::chain(range1, range2, range3);
    std::chrono::time_point end{std::chrono::system_clock::now()};

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "time taken: " << elapsed_seconds.count() << std::endl;
}

void test_generic_iterator()
{
    std::vector<int> vec{1, 2, 3, 4};
    auto vec_begin{vec.begin()};
    __itertools_utils::GenericIterator<int> begin{vec_begin};
    __itertools_utils::GenericIterator<int> end{vec.end()};

    ctest::assert_equal(*begin, 1);
    ++begin;
    ctest::assert_equal(*begin, 2);
    ctest::assert_equal(*vec_begin, 1);
    ctest::assert_equal(begin, begin);
    assert(begin != end);
    ++begin;
    ctest::assert_equal(*begin++, 3);
    ++begin;
    ctest::assert_equal(begin, end);

    // Check copies are independent iterators
    __itertools_utils::GenericIterator<int> orig{vec.begin()};
    __itertools_utils::GenericIterator<int> copy(orig);
    ++orig;
    ctest::assert_equal(*orig, 2);
    ctest::assert_equal(*copy, 1);

    // Check that it can be used by other functions
    ctest::assert_equal(std::distance(begin, end), 0);
    static_assert(std::input_iterator<__itertools_utils::GenericIterator<int>>);
}

void test_generic_range()
{
    std::vector<int> vec{1, 2, 3, 4};
    __itertools_utils::GenericRange<int> range(vec);
    ctest::assert_equal(std::distance(range.begin(), range.end()), 4);
    ctest::assert_equal(std::distance(range.begin(), range.end()), 4);
    for (const int item : range)
        assert(item >= 1);
    std::vector<int> extracted(range.begin(), range.end());
    ctest::assert_equal(extracted, vec);
    static_assert(std::ranges::input_range<__itertools_utils::GenericRange<int>>);
}

int main()
{
    test_slice();
    test_reversed();
    test_zip();
    test_range();
    test_pairwise();
    test_enumerate();
    test_init_last();
    test_head_tail();
    test_join();
    test_chain_class();
    test_chain();
    test_generic_iterator();
    test_generic_range();

    // test if the templating works for strings as well
    std::vector<int> int_vec{1, 2, 3, 4, 5};
    std::vector<std::string> str_vec{"this", "is", "a", "string", "vector", "cool"};
    auto sliced{itertools::slice(str_vec, -4, -1)};
    std::cout << itertools::to_vec(sliced) << std::endl;
    // std::cout << itertools::zip(sliced, int_vec) << std::endl;
    // std::cout << itertools::pairwise(str_vec) << std::endl;
    // std::cout << itertools::enumerate(str_vec) << std::endl;
    // std::cout << itertools::pairwise(itertools::zip(sliced, int_vec)) << std::endl;
    std::cout << itertools::join(str_vec, " ") << std::endl;
}