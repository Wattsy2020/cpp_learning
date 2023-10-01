#include <assert.h>
#include <vector>
#include <optional>
#include "doubly_linked_list.h"
#include "ctest.h"

void test_linked_list_add()
{
    LinkedList<int> list;
    list.add(1);
    ctest::assert_equal(list.back(), 1);
    ctest::assert_equal(list.size(), 1);
    ctest::assert_equal(list.items(), std::vector<int>{1});
    list.add(5);
    ctest::assert_equal(list.back(), 5);
    ctest::assert_equal(list.size(), 2);
    ctest::assert_equal(list.items(), std::vector<int>{1, 5});
}

void test_linked_list_access()
{
    LinkedList<int> list{1, 2, 3, 4};
    ctest::assert_equal(list.size(), 4);
    ctest::assert_equal(list[0], 1);
    ctest::assert_equal(list[1], 2);
    ctest::assert_equal(list[2], 3);
    ctest::assert_equal(list[3], 4);
    ctest::assert_equal(list.items(), std::vector<int>{1, 2, 3, 4});
    ctest::raises<std::range_error>([&list]()
                                    { list[-1]; },
                                    "Invalid index -1, must be between 0 and 4");
}

void test_linked_list_insert()
{
    LinkedList<int> list{1, 2, 3, 4};
    list.insert(0, 10);
    ctest::assert_equal(list.items(), std::vector<int>{10, 1, 2, 3, 4});

    list.insert(2, 5);
    ctest::assert_equal(list.items(), std::vector<int>{10, 1, 5, 2, 3, 4});

    // ensure insert can't insert at the last part of the list (add should be used instead)
    ctest::raises([&list]()
                  { list.insert(6, 10); });
}

void test_linked_list_remove()
{
    LinkedList<int> list{1, 2, 3, 4};
    list.remove(0);
    ctest::assert_equal(list.items(), std::vector<int>{2, 3, 4});

    list.remove(1);
    ctest::assert_equal(list.items(), std::vector<int>{2, 4});

    // ensure head is updated for one item list
    LinkedList<int> one_item{1};
    one_item.remove(0);
    ctest::assert_equal(one_item.items(), std::vector<int>{});

    // ensure tail is updated when removing this last one
    LinkedList<int> list2{1, 2, 3, 4};
    list2.remove(3);
    ctest::assert_equal(list2.back(), 3);
}

void test_linked_list_bool()
{
    assert(!LinkedList<int>{});
    assert(LinkedList<int>{1});
}

void test_linked_list_reverse()
{
    LinkedList<int> list{1, 2, 3, 4};
    list.reverse();
    ctest::assert_equal(list.items(), std::vector<int>{4, 3, 2, 1});

    list.reverse();
    ctest::assert_equal(list.items(), std::vector<int>{1, 2, 3, 4});

    list.insert(2, 10);
    ctest::assert_equal(list.items(), std::vector<int>{1, 2, 10, 3, 4});

    list.reverse();
    ctest::assert_equal(list.items(), std::vector<int>{4, 3, 10, 2, 1});
}

void test_linked_list_ends()
{
    LinkedList<int> list{1, 2, 3, 4};
    ctest::assert_equal(list.front(), std::make_optional<int>(1));
    ctest::assert_equal(list.back(), std::make_optional<int>(4));

    LinkedList<int> empty{};
    assert(!empty.back().has_value());
    assert(!empty.front().has_value());
}

void test_linked_list_end_manipulation()
{
    LinkedList<int> list{};
    list.add_left(1);
    list.add(2);
    list.add_left(0);
    ctest::assert_equal(list.items(), std::vector<int>{0, 1, 2});

    list.pop_left();
    list.pop();
    ctest::assert_equal(list.items(), std::vector<int>{1});
    ctest::assert_equal(list.size(), 1);
    ctest::assert_equal(list.pop(), 1);
    assert(!list.pop());
    list.add_left(-10);
    list.add(10);
    list.pop();
    list.add(15);
    list.add_left(-1);
    ctest::assert_equal(list.items(), std::vector<int>{-1, -10, 15});
    ctest::assert_equal(list.size(), 3);
}

int main()
{
    test_linked_list_add();
    test_linked_list_access();
    test_linked_list_insert();
    test_linked_list_remove();
    test_linked_list_bool();
    test_linked_list_reverse();
    test_linked_list_ends();
    test_linked_list_end_manipulation();
}
