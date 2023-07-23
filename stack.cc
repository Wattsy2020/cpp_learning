#include <assert.h>
#include <iostream>
#include <vector>
#include <exception>
#include <functional>
#include "itertools.h"
#include "testlib.h"

template <typename T>
class Stack
{
public:
    Stack<T>() : values{} {};
    Stack<T>(std::vector<T> init_values) : values{init_values} {};
    Stack<T>(std::initializer_list<T> init_values) : values{init_values} {};

    // add a copy of the item to the stack
    void add(const T item) { values.push_back(item); }

    T pop()
    {
        if (empty())
            throw std::length_error("Cannot pop from an empty stack");
        // note that values.back() is copied to last_elem through copy assignment
        // so last_elem should still exist even after calling pop_back
        T last_elem = {values.back()};
        values.pop_back();
        return last_elem;
    }

    void clear() { values.clear(); }

    bool empty() const { return values.empty(); }

    friend std::ostream &operator<<(std::ostream &os, const Stack<T> &stack)
    {
        os << stack.values;
        return os;
    }

    operator bool() const { return !values.empty(); }

private:
    std::vector<T> values;
};

void test_stack()
{
    Stack<int> stack{1, 2, 3, 4, 5};
    assert(stack.pop() == 5);
    assert(stack.pop() == 4);
    stack.add(10);
    stack.add(7);
    int result = stack.pop();
    assert(result == 7);
    assert(stack.pop() == 10);
    assert(stack.pop() == 3);
    assert(stack.pop() == 2);
    assert(stack.pop() == 1);
    assert(stack.empty());
    testlib::raises<std::length_error>(std::bind(&Stack<int>::pop, stack));
}

void test_stack_ostream()
{
    Stack<int> stack{1, 2, 3, 4, 5};
    testlib::assert_outstream(stack, "[ 1 2 3 4 5 ]");
}

void test_stack_bool()
{
    assert(!Stack<int>{});
    assert(Stack<int>{1});
}

int main()
{
    test_stack();
    test_stack_ostream();
    test_stack_bool();
}