#ifndef CPP_LEARNING_DOUBLY_LINKED_LIST
#define CPP_LEARNING_DOUBLY_LINKED_LIST

#include <memory>
#include <optional>
#include "itertools.h"

template <typename T>
class DoubleNode
{
public:
    DoubleNode(const T item = T{})
        : item{item},
          next_node{std::shared_ptr<DoubleNode<T>>{nullptr}},
          prev_node{std::weak_ptr<DoubleNode<T>>{}} {}

    void swap_order()
    {
        std::shared_ptr<DoubleNode<T>> temp_prev_node{prev_node.lock()};
        prev_node = next_node;
        next_node = temp_prev_node;
    }

    T item;
    std::shared_ptr<DoubleNode<T>> next_node;
    // weak_ptr to avoid shared_ptr cycles, so the DoublyLinkedList can be deallocated after going out of scope
    std::weak_ptr<DoubleNode<T>> prev_node;
};

template <typename T>
class LinkedList
{
public:
    using value_type = T;

    LinkedList() : length{0}
    {
        head = std::make_shared<DoubleNode<T>>();
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
        std::shared_ptr<DoubleNode<T>> new_node{std::make_shared<DoubleNode<T>>(item)};
        if (length == 0)
        {
            head->next_node = new_node;
            new_node->prev_node = head;
        }
        else
        {
            last->next_node = new_node;
            new_node->prev_node = last;
        }
        last = new_node;
        ++length;
    }

    // add an item and return a shared_ptr to its node
    std::shared_ptr<DoubleNode<T>> add_and_track(const T &item)
    {
        add(item);
        return last;
    }

    void add_left(const T item)
    {
        std::shared_ptr<DoubleNode<T>> new_node{std::make_shared<DoubleNode<T>>(item)};
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
        remove(last);
        return last_value;
    }

    std::optional<T> pop_left()
    {
        if (length == 0)
            return std::nullopt;
        T first_value{head->next_node->item};
        remove(head->next_node);
        return first_value;
    }

    // insert item anywhere in the list, so that becomes the item at index, and moves the previous item up. O(n)
    void insert(const int index, const T item)
    {
        itertools::validate_index(index, length);
        std::shared_ptr<DoubleNode<T>> prev_node{get_node(index - 1)};
        std::shared_ptr<DoubleNode<T>> new_node{std::make_shared<DoubleNode<T>>(item)};
        std::shared_ptr<DoubleNode<T>> next_node{prev_node->next_node};
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
        remove(get_node(index));
    }

    // remove a node in this linkedlist
    // note: doesn't check the node is contained in the linkedlist
    void remove(const std::shared_ptr<DoubleNode<T>> &node)
    {
        std::shared_ptr<DoubleNode<T>> prev_node{node->prev_node};
        std::shared_ptr<DoubleNode<T>> following_node{node->next_node};
        if (!following_node)
            last = prev_node; // removing the last item, so update it to point to prev_node
        else
            following_node->prev_node = prev_node;
        prev_node->next_node = following_node;
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
        std::shared_ptr<DoubleNode<T>> current = head->next_node;
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

        std::shared_ptr<DoubleNode<T>> current{last};
        while (current)
        {
            current->swap_order();
            current = current->next_node;
        }
        std::swap(head, last);

        // after swapping, tail will point to one past the last element, so remove that
        std::shared_ptr<DoubleNode<T>> new_last{last->prev_node};
        new_last->next_node = nullptr;
        last = new_last;

        // head needs to point to one before the first node
        auto new_head = std::make_shared<DoubleNode<T>>();
        new_head->next_node = head;
        head->prev_node = new_head;
        head = new_head;
    }

    int size() const { return length; }

    operator bool() const { return length > 0; }

private:
    std::shared_ptr<DoubleNode<T>> head; // points to 1 node before the first item
    std::shared_ptr<DoubleNode<T>> last; // points to the last item
    int length;

    std::shared_ptr<DoubleNode<T>> get_node(const int index)
    {
        assert(index >= -1 && index < length);
        std::shared_ptr<DoubleNode<T>> current = head;
        for (int i = -1; i < index; ++i)
            current = current->next_node;
        return current;
    }
};

#endif
