#include <assert.h>
#include "set.h"
#include "testlib.h"

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
    assert(set.items() == std::vector<int>{1});
}

void test_set_remove()
{
    Set<int> set{1, 2, 100001};
    set.remove(1);
    assert(set.items() == (std::vector<int>{2, 100001}));
    set.remove(3); // ensure no error is thrown
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
    assert(set.items() == std::vector<int>{});
    set.add(1);
    set.add(1004);
    assert(set.items() == (std::vector<int>{1, 1004}));
}

void test_set_iteration()
{
    Set<int> set{1};
    for (int item : set)
        assert(item == 1);

    Set<int> to_sum{1, 2, 3, 4};
    assert(std::accumulate(to_sum.begin(), to_sum.end(), 0) == 10);

    Set<int> sliced{itertools::slice(to_sum, 0, -1)};
    assert(sliced.items() == (std::vector<int>{1, 2, 3}));

    /* TODO: update Set<int> to be a proper iterator, so these tests pass
    std::vector<std::tuple<int, std::string>> zipped{itertools::zip(to_sum, std::vector<std::string>{"hello", "there"})};
    std::vector<std::tuple<int, std::string>> expected_result{std::vector{std::make_tuple(1, std::string("hello")), std::make_tuple(2, std::string("there"))}};
    assert(zipped == expected_result);
    */

    // below works when returning T*, fails for const T*
    // trying to define T *begin() const fails, as the compiler recognizes returning a non const T* means the function cannot be const
    //*set.begin() = 5;
    // assert(set.items() == (std::vector<int>{5}));
}

void test_set_outstream()
{
    testlib::assert_outstream(Set<int>{}, "{ }");
    testlib::assert_outstream(Set<int>{1, 1004}, "{ 1 1004 }");
}

void test_vector_constructor()
{
    std::vector<int> input_vec{1, 10004};
    Set<int> set(input_vec);
    assert(set.items() == input_vec);
}

void test_initialiser_list_constructor()
{
    Set<int> set{1, 10004};
    assert(set.items() == (std::vector<int>{1, 10004}));
}

void test_hash_conflict()
{
    // using a small set size would cause hash conflicts, test if the set class still stores all the items
    std::vector<int> long_vector{itertools::range(1, 1000)};
    Set<int> set(long_vector, std::identity(), 10);
    for (const int &i : long_vector)
        assert(set.contains(i));
}

void test_set_union()
{
    Set<int> set1{1, 2, 3};
    Set<int> set2{2, 3, 4};
    Set<int> unioned_set{set::set_union(set1, set2)};
    assert(unioned_set.items() == (std::vector<int>{1, 2, 3, 4}));
}

void test_set_intersection()
{
    Set<int> set1{-1, 0, 1, 2, 3};
    Set<int> set2{2, 3, 4, 5};
    Set<int> result{set::intersection(set1, set2)};
    assert(result.items() == (std::vector<int>{2, 3}));
}

void test_set_difference()
{
    Set<int> set1{-1, 0, 1, 2, 3};
    Set<int> set2{2, 3, 4, 5};

    Set<int> diff1{set::difference(set1, set2)};
    assert(diff1.items() == (std::vector<int>{-1, 0, 1}));

    Set<int> diff2{set::difference(set2, set1)};
    assert(diff2.items() == (std::vector<int>{4, 5}));
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
    assert(set.size() == 2);
    assert(set.items() == (std::vector{vec1, vec2}));

    assert(set.get(5) == vec1);
    std::vector<int> new_vec{1, 2, 3, 5};
    set.set(5, new_vec);
    assert(set.get(5) == new_vec);
}

int main()
{
    test_set_add();
    test_set_remove();
    test_set_contains();
    test_set_items();
    test_set_outstream();
    test_set_iteration();
    test_vector_constructor();
    test_hash_conflict();
    test_set_union();
    test_set_intersection();
    test_set_difference();
    test_set_is_subset();
    test_set_is_superset();
    test_set_key_func();
}