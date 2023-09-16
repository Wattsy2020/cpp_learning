#include <memory>
#include <assert.h>
#include <optional>
#include "itertools.h"
#include "testlib.h"

namespace __node
{
    template <typename T>
    class DoubleNode
    {
    public:
        DoubleNode(const T item = T{})
            : item{item},
              next_node{std::shared_ptr<DoubleNode<T>>{nullptr}},
              prev_node{std::weak_ptr<DoubleNode<T>>{}} {}
        T item;
        std::shared_ptr<DoubleNode<T>> next_node;
        // weak_ptr to avoid shared_ptr cycles, so the DoublyLinkedList can be deallocated after going out of scope
        std::weak_ptr<DoubleNode<T>> prev_node;
    };
}

template <typename T>
class LinkedList
{
public:
    using value_type = T;

    LinkedList() : length{0}
    {
        head = std::make_shared<__node::DoubleNode<T>>();
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
        std::shared_ptr<__node::DoubleNode<T>> new_node{std::make_shared<__node::DoubleNode<T>>(item)};
        if (length == 0)
            head->next_node = new_node;
        last->next_node = new_node;
        new_node->prev_node = last;
        last = new_node;
        ++length;
    }

    void add_left(const T item)
    {
        std::shared_ptr<__node::DoubleNode<T>> new_node{std::make_shared<__node::DoubleNode<T>>(item)};
        if (length == 0)
        {
            last = new_node;
            head->next_node = new_node;
        }
        else
        {
            new_node->next_node = head->next_node;
            head->next_node->prev_node = new_node;
            head->next_node = new_node;
        }
        new_node->prev_node = head;
        ++length;
    }

    // remove and return the last item in the list. If the list is empty returns nullopt
    std::optional<T> pop()
    {
        if (length == 0)
            return std::nullopt;
        T last_value{last->item};
        last = last->prev_node.lock();
        last->next_node = nullptr;
        return last_value;
    }

    std::optional<T> pop_left()
    {
        if (length == 0)
            return std::nullopt;
        T first_value{head->next_node->item};
        auto new_first_node{head->next_node->next_node};
        new_first_node->prev_node = head;
        head->next_node = new_first_node;
        return first_value;
    }

    // insert item anywhere in the list, so that becomes the item at index, and moves the previous item up. O(n)
    void insert(const int index, const T item)
    {
        itertools::validate_index(index, length);
        std::shared_ptr<__node::DoubleNode<T>> prev_node{get_node(index - 1)};
        std::shared_ptr<__node::DoubleNode<T>> new_node{std::make_shared<__node::DoubleNode<T>>(item)};
        std::shared_ptr<__node::DoubleNode<T>> next_node{prev_node->next_node};
        prev_node->next_node = new_node;
        new_node->next_node = next_node;

        next_node->prev_node = new_node;
        new_node->prev_node = prev_node;
        ++length;
    }

    // remove item anywhere in the list, O(n)
    void remove(const int index)
    {
        itertools::validate_index(index, length);
        std::shared_ptr<__node::DoubleNode<T>> prev_node{get_node(index - 1)};
        std::shared_ptr<__node::DoubleNode<T>> following_node{prev_node->next_node->next_node};
        if (!following_node)
            last = prev_node; // removing the last item, so update it to point to prev_node
        prev_node->next_node = following_node;
        if (following_node)
            following_node->prev_node = prev_node;
        --length;
    }

    std::optional<T> front() const
    {
        return (length > 0) ? std::make_optional<T>(head->next_node->item) : std::nullopt;
    }

    std::optional<T> back() const
    {
        return (length > 0) ? std::make_optional<T>(last->item) : std::nullopt;
    }

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
        std::shared_ptr<__node::DoubleNode<T>> current = head->next_node;
        while (current)
        {
            result.push_back(current->item);
            current = current->next_node;
        }
        return result;
    }

    void reverse()
    {
        if (length <= 1)
            return;

        std::shared_ptr<__node::DoubleNode<T>> current{last};
        std::shared_ptr<__node::DoubleNode<T>> next_node;
        while (current)
        {
            next_node = current->next_node;
            current->next_node = current->prev_node.lock();
            current->prev_node = next_node;
            current = current->next_node;
        }

        // swap head and tail
        std::swap(head, last);
        pop(); // after swapping, tail will point to one past the last element, so remove that
        auto new_head = std::make_shared<__node::DoubleNode<T>>();
        new_head->next_node = head;
        head->prev_node = new_head;
        head = new_head;
    }

    int size() const { return length; }

    operator bool() { return length > 0; }

private:
    std::shared_ptr<__node::DoubleNode<T>> head; // points to 1 node before the first item
    std::shared_ptr<__node::DoubleNode<T>> last; // points to the last item
    int length;

    std::shared_ptr<__node::DoubleNode<T>> get_node(const int index)
    {
        assert(index >= -1 && index < length);
        std::shared_ptr<__node::DoubleNode<T>> current = head;
        for (int i = -1; i < index; ++i)
            current = current->next_node;
        return current;
    }
};

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
    testlib::raises<std::range_error>([&list]()
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
    testlib::raises([&list]()
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

void test_linked_list_reverse()
{
    LinkedList<int> list{1, 2, 3, 4};
    list.reverse();
    std::vector<int> result{list.items()};
    assert(result == (std::vector<int>{4, 3, 2, 1}));

    list.reverse();
    assert(list.items() == (std::vector<int>{1, 2, 3, 4}));

    list.insert(2, 10);
    assert(list.items() == (std::vector<int>{1, 2, 10, 3, 4}));

    list.reverse();
    assert(list.items() == (std::vector<int>{4, 3, 10, 2, 1}));
}

void test_linked_list_ends()
{
    LinkedList<int> list{1, 2, 3, 4};
    assert(list.front() == std::make_optional<int>(1));
    assert(list.back() == std::make_optional<int>(4));

    LinkedList<int> empty{};
    assert(empty.front() == std::nullopt);
    assert(empty.back() == std::nullopt);
}

void test_linked_list_end_manipulation()
{
    LinkedList<int> list{};
    list.add_left(1);
    list.add(2);
    list.add_left(0);
    std::vector<int> result{list.items()};
    assert(result == (std::vector<int>{0, 1, 2}));

    list.pop_left();
    list.pop();
    list.add_left(-10);
    list.add(10);
    list.pop();
    list.add(15);
    list.add_left(-1);
    std::vector<int> result2{list.items()};
    assert(result2 == (std::vector<int>{-1, -10, 1, 15}));
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
