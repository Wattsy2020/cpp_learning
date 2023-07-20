#include <assert.h>
#include <iostream>
#include "itertools.h"

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

    // TODO: add move and copy constructors

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

    // Double the capacity for the queue, while also removing items that have exited the queue
    void increase_capacity()
    {
        array_end_offset *= 2;
        T *larger_values_ptr = new T[array_end_offset];

        // copying from queue_start_offset onwards will leave behind the items that have already been removed from the queue
        std::copy(values_ptr + queue_start_offset, values_ptr + queue_end_offset, larger_values_ptr);
        queue_end_offset -= queue_start_offset; // since we deleted the items before queue_start_offset, shift the offsets back
        queue_start_offset = 0;

        delete[] values_ptr;
        values_ptr = larger_values_ptr;
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

// test copy constructor, ensure copies don't affect each other

// same test for copy assignment

int main()
{
    test_queue();
    test_queue_initializer_list();
}