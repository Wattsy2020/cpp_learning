#include <memory>
#include <exception>
#include <vector>
#include <tuple>
#include "itertools.h"
#include "ctest.h"
#include <assert.h>

namespace __node
{
    template <typename T>
    class Node
    {
    public:
        Node(const T item = T{}) : item{item}, next_node{std::shared_ptr<Node<T>>{nullptr}} {}
        T item;
        std::shared_ptr<Node<T>> next_node;
    };
}

template <typename T>
class Iterator
{
public:
    // typedefs are for compatibility with standard library algorithms
    // https://medium.com/@joao_vaz/c-iterators-and-implementing-your-own-custom-one-a-primer-72f1506e5d71
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::forward_iterator_tag;

    Iterator() : current{nullptr} {}
    Iterator(std::shared_ptr<__node::Node<T>> head) : current{head} {}

    // Forward iterator needs to support equality, incrementation, and de-referencing https://www.javatpoint.com/cpp-forward-iterator
    friend bool operator==(const Iterator<T> &iter1, const Iterator<T> &iter2) { return iter1.current == iter2.current; }
    friend bool operator!=(const Iterator<T> &iter1, const Iterator<T> &iter2) { return !(iter1.current == iter2.current); }

    reference operator*() const { return current->item; }

    Iterator &operator++()
    {
        current = current->next_node;
        return *this;
    }

    Iterator operator++(int)
    {
        Iterator temp = *this;
        current = current->next_node;
        return temp;
    }

private:
    std::shared_ptr<__node::Node<T>> current;
};

template <typename T>
class LinkedList
{
public:
    using value_type = T;
    using iterator = Iterator<T>;
    using const_iterator = Iterator<T>;

    LinkedList() : length{0}
    {
        head = std::make_shared<__node::Node<T>>();
        last = head;
    }

    LinkedList(const std::initializer_list<T> &items) : LinkedList()
    {
        for (const T item : items)
            add(item);
    }

    // add item to the linked list, O(1)
    void add(const T item)
    {
        // note: need to update head if this is the first item
        // then update last to point to the new item
        auto new_node{std::make_shared<__node::Node<T>>(item)};
        if (!head->next_node)
            head->next_node = new_node;
        last->next_node = new_node;
        last = new_node;
        ++length;
    }

    // insert item anywhere in the list, so that becomes the item at index, and moves the previous item up. O(n)
    void insert(const int index, const T item)
    {
        itertools::validate_index(index, length);
        std::shared_ptr<__node::Node<T>> prev_node{get_node(index - 1)};
        std::shared_ptr<__node::Node<T>> new_node{std::make_shared<__node::Node<T>>(item)};
        new_node->next_node = prev_node->next_node;
        prev_node->next_node = new_node;
        ++length;
    }

    // remove item anywhere in the list, O(n)
    void remove(const int index)
    {
        itertools::validate_index(index, length);
        std::shared_ptr<__node::Node<T>> prev_node{get_node(index - 1)};
        std::shared_ptr<__node::Node<T>> following_node{prev_node->next_node->next_node};
        if (!following_node)
            last = prev_node; // removing the last item, so update it to point to prev_node
        prev_node->next_node = following_node;
        --length;
    }

    T back() const { return last->item; }

    // get item, note this is O(n) and inefficient
    T &operator[](const int index)
    {
        itertools::validate_index(index, length);
        return get_node(index)->item;
    }

    // get all items, for efficient access
    std::vector<T> items() const
    {
        std::vector<T> result{};
        std::shared_ptr<__node::Node<T>> current = head->next_node;
        while (current)
        {
            result.push_back(current->item);
            current = current->next_node;
        }
        return result;
    }

    int size() const { return length; }

    operator bool() { return size() > 0; }

    Iterator<T> begin() const { return Iterator<T>(head->next_node); }

    Iterator<T> end() const { return Iterator<T>(); }

private:
    std::shared_ptr<__node::Node<T>> head; // points to 1 node before the first item
    std::shared_ptr<__node::Node<T>> last; // points to the last item
    int length;

    std::shared_ptr<__node::Node<T>> get_node(const int index)
    {
        assert(index >= -1 && index < length);
        std::shared_ptr<__node::Node<T>> current = head;
        for (int i = -1; i < index; ++i)
            current = current->next_node;
        return current;
    }
};

void test_node()
{
    __node::Node<int> node(1);
    assert(node.item == 1);
    assert(node.next_node == nullptr);
    node.next_node.reset(new __node::Node<int>(2));
    assert(node.next_node->item == 2);
}

void test_linked_list_add()
{
    LinkedList<int> list;
    list.add(1);
    assert(list.back() == 1);
    assert(list.size() == 1);
    assert(list.items() == (std::vector<int>{1}));
    list.add(5);
    assert(list.back() == 5);
    assert(list.size() == 2);
    assert(list.items() == (std::vector<int>{1, 5}));
}

void test_linked_list_access()
{
    LinkedList<int> list{1, 2, 3, 4};
    assert(list.size() == 4);
    assert(list[0] == 1);
    assert(list[1] == 2);
    assert(list[2] == 3);
    assert(list[3] == 4);
    assert(list.items() == (std::vector<int>{1, 2, 3, 4}));
    ctest::raises<std::range_error>([&list]()
                                      { list[-1]; },
                                      "Invalid index -1, must be between 0 and 4");
}

void test_linked_list_insert()
{
    LinkedList<int> list{1, 2, 3, 4};
    list.insert(0, 10);
    std::vector<int> result{list.items()};
    assert(result == (std::vector<int>{10, 1, 2, 3, 4}));

    list.insert(2, 5);
    std::vector<int> result2{list.items()};
    assert(result2 == (std::vector<int>{10, 1, 5, 2, 3, 4}));

    // ensure insert can't insert at the last part of the list (add should be used instead)
    ctest::raises([&list]()
                    { list.insert(6, 10); });
}

void test_linked_list_remove()
{
    LinkedList<int> list{1, 2, 3, 4};
    list.remove(0);
    std::vector<int> result{list.items()};
    assert(result == (std::vector<int>{2, 3, 4}));

    list.remove(1);
    std::vector<int> result2{list.items()};
    assert(result2 == (std::vector<int>{2, 4}));

    // ensure head is updated for one item list
    LinkedList<int> one_item{1};
    one_item.remove(0);
    std::vector<int> result3{one_item.items()};
    assert(result3 == (std::vector<int>{}));

    // ensure tail is updated when removing this last one
    LinkedList<int> list2{1, 2, 3, 4};
    list2.remove(3);
    assert(list2.back() == 3);
}

void test_linked_list_bool()
{
    assert(!LinkedList<int>{});
    assert(LinkedList<int>{1});
}

void test_linked_list_iterator()
{
    LinkedList<int> list{1, 2, 3, 4};
    std::vector<int> extracted(list.begin(), list.end());
    assert(extracted == (std::vector<int>{1, 2, 3, 4}));

    // Check concepts
    static_assert(std::forward_iterator<Iterator<int>>);
    static_assert(std::ranges::forward_range<LinkedList<int>>);

    // Test modifying values works
    *(++list.begin()) = 5;
    assert(list[0] == 1);
    assert(list[1] == 5);

    // Test itertools algorithms work
    std::vector<std::tuple<int, int>> enumerated{itertools::enumerate(list)};
    std::vector<std::tuple<int, int>> expected_result{{0, 1}, {1, 5}, {2, 3}, {3, 4}};
    assert(enumerated == expected_result);
}

int main()
{
    test_node();
    test_linked_list_add();
    test_linked_list_access();
    test_linked_list_insert();
    test_linked_list_remove();
    test_linked_list_bool();
    test_linked_list_iterator();
}
