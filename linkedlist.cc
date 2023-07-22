#include <memory>
#include <exception>

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

    // add item to the linked list
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

    T back() const { return last->item; }

    const T &operator[](int index) const
    {
        if (index < 0 || index >= length)
            throw std::range_error("Invalid index, must be between 0 and length");

        std::shared_ptr<__node::Node<T>> current = head.next_node;
        for (int i = 0; i < index; ++i)
            current = current->next_node;
        return current->item;
    }

private:
    __node::Node<T> head;
    std::shared_ptr<__node::Node<T>> last;
    int length;
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
}

int main()
{
    test_node();
    test_linked_list_add();
    test_linked_list_access();
}
