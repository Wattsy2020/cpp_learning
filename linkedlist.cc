#include <memory>

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
    LinkedList() : head{__node::Node<T>()}
    {
        last = std::make_shared<__node::Node<T>>(head);
    }

    void add(const T item)
    {
        last->next_node = std::make_shared<__node::Node<T>>(item);
        last = last->next_node;
    }

    T back() const { return last->item; }

private:
    __node::Node<T> head;
    std::shared_ptr<__node::Node<T>> last;
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
    list.add(2);
    assert(list.back() == 2);
}

int main()
{
    test_node();
    test_linked_list_add();
}
