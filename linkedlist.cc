#include <memory>
#include <exception>
#include <vector>
#include "itertools.h"
#include "testlib.h"

namespace __node
{
    template <typename T>
    class Node
    {
    public:
        Node() : item{}, next_node{std::shared_ptr<Node<T>>{nullptr}} {}
        Node(const T item) : item{item}, next_node{std::shared_ptr<Node<T>>{nullptr}} {}
        T item;
        std::shared_ptr<Node<T>> next_node;
    };
}

template <typename T>
class LinkedList
{
public:
    LinkedList() : head{__node::Node<T>()}, length{0}
    {
        last = std::make_shared<__node::Node<T>>(head);
    }

    LinkedList(const std::initializer_list<T> &items) : LinkedList()
    {
        for (const T item : items)
            add(item);
    }

    int size() const { return length; }

    // add item to the linked list, O(1)
    void add(const T item)
    {
        // note: need to update head if this is the first item
        // then update last to point to the new item
        auto new_node = std::make_shared<__node::Node<T>>(item);
        if (!head.next_node)
            head.next_node = new_node;
        last->next_node = new_node;
        last = new_node;
        ++length;
    }

    void insert(int index, const T item)
    {
        itertools::validate_index(index, length);
        __node::Node<T> &prev_node = (index == 0) ? head : get_node(index - 1);
        __node::Node<T> new_node{__node::Node<T>(item)};
        new_node.next_node = prev_node.next_node;
        prev_node.next_node = std::make_shared<__node::Node<T>>(new_node);
        ++length;
    }

    T back() const { return last->item; }

    // get item, note this is O(n) and inefficient
    T &operator[](const int index)
    {
        itertools::validate_index(index, length);
        return get_node(index).item;
    }

    // get all items, for efficient access
    std::vector<T> items() const
    {
        std::vector<T> result{};
        std::shared_ptr<__node::Node<T>> current = head.next_node;
        for (int i = 0; i < length; ++i)
        {
            result.push_back(current->item);
            current = current->next_node;
        }
        return result;
    }

private:
    __node::Node<T> head;
    std::shared_ptr<__node::Node<T>> last;
    int length;

    __node::Node<T> &get_node(const int index)
    {
        assert(index >= 0 && index < length);
        std::shared_ptr<__node::Node<T>> current = head.next_node;
        for (int i = 0; i < index; ++i)
            current = current->next_node;
        return *current;
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
    list.add(5);
    assert(list.back() == 5);
    assert(list.size() == 2);
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
    testlib::raises<std::range_error>([&list]()
                                      { list[-1]; });
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
}

int main()
{
    test_node();
    test_linked_list_add();
    test_linked_list_access();
    test_linked_list_insert();
}
