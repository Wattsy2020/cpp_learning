#include <assert.h>
#include <exception>
#include <iostream>
#include <optional>
#include "itertools.h"

// Deliberately implemented without smart pointers to practice RAII with pointers
template <typename T>
class Queue
{
public:
    Queue(const int capacity = 128)
        : values_ptr{new T[capacity]},
          queue_start_offset{0},
          queue_end_offset{0},
          array_end_offset{capacity} {};

    Queue(const std::initializer_list<T> &items, const int capacity = 128) : Queue(capacity)
    {
        for (const T &item : items)
            push_back(item);
    }

    // Copy another queue, ignoring all lazily removed items
    // copies the capacity of the other queue by default, can also increase capacity if specified
    Queue(const Queue &other, const std::optional<int> capacity = std::nullopt)
        : Queue(capacity.value_or(other.array_end_offset))
    {
        if (capacity.value_or(other.array_end_offset) < other.capacity())
            throw std::length_error("Cannot copy a queue to another queue with smaller capacity");

        // copying from queue_start_offset onwards will leave behind the items that have already been removed from the queue
        std::copy(other.values_ptr + other.queue_start_offset, other.values_ptr + other.queue_end_offset, values_ptr);
        queue_end_offset = other.size();
    }

    Queue &operator=(const Queue &other)
    {
        // copy other into a temporary, then swap, so that the temporary variable's destructor deallocates this->values_ptr
        Queue(other).swap(*this);
        return *this;
    }

    Queue(Queue &&other)
    {
        other.swap(*this);
        other.values_ptr = nullptr; // so destructor is called on a nullptr, instead of un-initialised pointer
    }

    Queue &operator=(Queue &&other)
    {
        // swap *this into a temporary variable, so that this.values_ptr is deallocated immediately after the move assignment finishes
        Queue(std::move(other)).swap(*this);
        other.values_ptr = nullptr;
        return *this;
    }

    ~Queue()
    {
        delete[] values_ptr;
        values_ptr = nullptr;
    }

    void push_back(const T item)
    {
        if (queue_end_offset == array_end_offset)
            increase_capacity();
        values_ptr[queue_end_offset] = item;
        ++queue_end_offset;
    }

    // return (and lazily remove) the head of the queue
    T pop_head() { return values_ptr[queue_start_offset++]; }

    int capacity() const { return array_end_offset; }

    int size() const { return queue_end_offset - queue_start_offset; }

private:
    T *values_ptr;
    int queue_start_offset; // values_ptr + queue_start_offset == the first element in the queue
    int queue_end_offset;   // values_ptr + queue_end_offset == 1 past the last element in the queue
    int array_end_offset;   // values_ptr + array_end_offset == 1 past the last element in the array

    // move and swap pattern: create a temporary other, swap variables in this with other
    // then other goes out of scope, calling the destructor for the old variables in this
    void swap(Queue &other) noexcept
    {
        std::swap(this->values_ptr, other.values_ptr);
        std::swap(this->queue_start_offset, other.queue_start_offset);
        std::swap(this->queue_end_offset, other.queue_end_offset);
        std::swap(this->array_end_offset, other.array_end_offset);
    }

    // Double the capacity for the queue, while also removing items that have exited the queue
    void increase_capacity()
    {
        // Create a new Queue with double the capacity using the copy constructor
        // the copy assignment then ensures that the old values in *this are deallocated by the destructor
        Queue(*this, array_end_offset * 2).swap(*this);
    }
};

void test_queue()
{
    Queue<int> queue(1);
    assert(queue.size() == 0);
    assert(queue.capacity() == 1);

    queue.push_back(1);
    assert(queue.size() == 1);
    assert(queue.capacity() == 1);

    queue.push_back(2);
    assert(queue.size() == 2);
    assert(queue.capacity() == 2);

    queue.push_back(3);
    assert(queue.size() == 3);
    assert(queue.capacity() == 4);

    assert(queue.pop_head() == 1);
    assert(queue.size() == 2);
    assert(queue.pop_head() == 2);
    assert(queue.size() == 1);
    assert(queue.pop_head() == 3);
    assert(queue.size() == 0);

    // now test that the popped items are removed when increasing the capacity
    queue.push_back(4);
    assert(queue.size() == 1);
    queue.push_back(5);
    assert(queue.size() == 2);
    assert(queue.capacity() == 8);

    for (const int i : itertools::range(6, 12))
        queue.push_back(i);
    assert(queue.size() == 8);
    assert(queue.capacity() == 8);

    while (queue.size())
        std::cout << queue.pop_head() << std::endl;
    assert(queue.size() == 0);
    assert(queue.capacity() == 8);
}

void test_queue_initializer_list()
{
    Queue<int> queue{5, 2, 3, 4, 5, 6};
    assert(queue.size() == 6);
    assert(queue.pop_head() == 5);
    assert(queue.size() == 5);
}

void test_queue_copy_constructor()
{
    // ensure modifying copies don't affect each other
    Queue<int> queue1{1, 2, 3};
    Queue<int> queue2(queue1);
    assert(queue1.size() == 3);
    assert(queue1.pop_head() == 1);
    assert(queue1.size() == 2);
    assert(queue2.size() == 3);
    assert(queue2.pop_head() == 1);
    assert(queue2.size() == 2);

    // ensure copying removes lazily deleted items
    Queue<int> queue3(2);
    queue3.push_back(1);
    queue3.push_back(2);
    assert(queue3.pop_head() == 1);
    assert(queue3.size() == 1);
    Queue<int> queue4(queue3, 2);
    assert(queue4.size() == 1);
    queue4.push_back(3);
    assert(queue4.size() == 2);
    assert(queue4.capacity() == 2);
    assert(queue4.pop_head() == 2);
}

void test_queue_copy_assignment()
{
    Queue<int> queue1{1, 2, 3};
    Queue<int> queue2{};
    queue2 = queue1;
    assert(queue1.size() == 3);
    assert(queue1.pop_head() == 1);
    assert(queue1.size() == 2);
    assert(queue2.size() == 3);
    assert(queue2.pop_head() == 1);
    assert(queue2.size() == 2);
}

void test_queue_move_assignment()
{
    Queue<int> queue1{1, 2, 3};
    Queue<int> queue2{};
    queue2 = std::move(queue1); // now queue1 is invalid
    assert(queue2.size() == 3);
    assert(queue2.pop_head() == 1);
    // assert(queue1.values_ptr == nullptr); // cannot even run this, get a compiler error
    queue2.push_back(4);
    for (const int queue_val : itertools::range(2, 5))
        assert(queue2.pop_head() == queue_val);
}

int main()
{
    test_queue();
    test_queue_initializer_list();
    test_queue_copy_constructor();
    test_queue_copy_assignment();
    test_queue_move_assignment();
}