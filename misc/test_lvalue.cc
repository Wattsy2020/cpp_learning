#include <iostream>

int &create_lvalue(int &lvalue)
{
    lvalue *= 2;
    return lvalue;
    // return 3; // compiler error: must be an lvalue
}

int main()
{
    int x = 2;
    create_lvalue(x) = 3;
    std::cout << x << std::endl;
}