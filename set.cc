#include <ranges>
#include <assert.h>
#include <vector>
#include "set.h"
#include "ctest.h"

void test_set_add()
{
    Set<int> set{};
    assert(!set);
    set.add(1);
    assert(set);
    assert(set.contains(1));

    // test duplicates aren't added twice
    set.add(1);
    assert(set.contains(1));
    ctest::assert_equal(set.items(), std::vector<int>{1});
}

void test_set_remove()
{
    Set<int> set{1, 2, 100001};
    set.remove(2);
    ctest::assert_equal(set.items(), std::vector<int>{1, 100001});
    set.remove(3); // ensure no error is thrown

    set.remove(1);
    set.remove(100001);
    set.add(4);
    ctest::assert_equal(set.items(), std::vector<int>{4});
}

void test_set_contains()
{
    Set<int> set{};
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
    Set<int> set{};
    ctest::assert_equal(set.items(), std::vector<int>{});
    set.add(1);
    set.add(1004);
    ctest::assert_equal(set.items(), std::vector<int>{1, 1004});
}

void test_set_outstream()
{
    ctest::assert_outstream(Set<int>{}, "{ }");
    ctest::assert_outstream(Set<int>{1, 1004}, "{ 1 1004 }");
}

void test_vector_constructor()
{
    std::vector<int> input_vec{1, 10004};
    Set<int> set(input_vec);
    ctest::assert_equal(set.items(), input_vec);
}

void test_initialiser_list_constructor()
{
    Set<int> set{1, 10004};
    ctest::assert_equal(set.items(), std::vector<int>{1, 10004});
}

void test_capacity_expansion()
{
    std::vector<int> long_vector{itertools::range(1, 1000)};
    Set<int> set(long_vector, std::identity());
    for (const int &i : long_vector)
        assert(set.contains(i));
    assert(set.capacity() > 1000);
}

void test_set_union()
{
    Set<int> set1{1, 2, 3};
    Set<int> set2{2, 3, 4};
    Set<int> unioned_set{set::set_union(set1, set2)};
    ctest::assert_equal(unioned_set.items(), std::vector<int>{1, 2, 3, 4});
}

void test_set_intersection()
{
    Set<int> set1{-1, 0, 1, 2, 3};
    Set<int> set2{2, 3, 4, 5};
    Set<int> result{set::intersection(set1, set2)};
    ctest::assert_equal(result.items(), std::vector<int>{2, 3});
}

void test_set_difference()
{
    Set<int> set1{-1, 0, 1, 2, 3};
    Set<int> set2{2, 3, 4, 5};

    Set<int> diff1{set::difference(set1, set2)};
    ctest::assert_equal(diff1.items(), std::vector<int>{-1, 0, 1});

    Set<int> diff2{set::difference(set2, set1)};
    ctest::assert_equal(diff2.items(), std::vector<int>{4, 5});
}

void test_set_is_subset()
{
    Set<int> set1{2, 3};
    Set<int> set2{2, 3, 4, 5};
    assert(set::is_subset(set1, set2));
    assert(!set::is_subset(set2, set1));
}

void test_set_is_superset()
{
    Set<int> set1{2, 3};
    Set<int> set2{2, 3, 4, 5};
    assert(!set::is_superset(set1, set2));
    assert(set::is_superset(set2, set1));
}

void test_set_equality()
{
    Set<int> set1{2, 3};
    Set<int> set2{2, 3, 4, 5};
    Set<int> set3{3};
    Set<int> set4{3, 2};
    assert(set1 != set2);
    assert(set1 != set3);
    assert(set2 != set3);
    ctest::assert_equal(set1, set4);
}

void test_set_key_func()
{
    // add unhashable type, but with a function that converts it to a hashable type
    std::function<int(std::vector<int>)> key_func{[](const std::vector<int> &vec)
                                                  { return vec.back(); }};
    Set<int, std::vector<int>> set(key_func);
    std::vector<int> vec1{1, 2, 5};
    std::vector<int> vec2{1, 2};
    set.add(vec1);
    set.add(vec2);
    assert(set.contains(vec1));
    assert(set.contains(vec2));
    assert(!set.contains(std::vector<int>{1}));
    ctest::assert_equal(set.size(), 2);
    ctest::assert_equal(set.items(), std::vector{vec1, vec2});

    ctest::assert_equal(set.get(5), vec1);
    std::vector<int> new_vec{1, 2, 3, 5};
    set.set(5, new_vec);
    ctest::assert_equal(set.get(5), new_vec);
}

void test_set_insertion_order()
{
    Set<int> set1{};
    set1.add(1000);
    set1.add(-400);
    set1.add(2);
    set1.add(1);
    ctest::assert_equal(set1.items(), std::vector{1000, -400, 2, 1});
}

int main()
{
    test_set_add();
    test_set_remove();
    test_set_contains();
    test_set_items();
    test_set_outstream();
    test_vector_constructor();
    test_capacity_expansion();
    test_set_union();
    test_set_intersection();
    test_set_difference();
    test_set_is_subset();
    test_set_is_superset();
    test_set_equality();
    test_set_key_func();
    test_set_insertion_order();
}