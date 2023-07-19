#include <iostream>

int main()
{
    int *pointer = new int[5];
    pointer[2] = 10;
    int &int_value = pointer[2];
    std::cout << int_value << std::endl; // 10
    delete[] pointer;
    std::cout << int_value << std::endl; // 0 after deallocation
    // lesson: if you have a reference to some memory, when that memory is deallocated the value referred to may change
}