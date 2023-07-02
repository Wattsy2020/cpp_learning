#include <vector>
#include <iostream>
#include <tuple>
#include "itertools.h"

int main()
{
    std::vector<int> test_vec{1, 2, 3, 4, 5};
    std::cout << slice(test_vec, 0, 2) << std::endl;
    std::cout << slice(test_vec, 2, 3) << std::endl;
    std::cout << slice(test_vec, 0, 0) << std::endl;
    std::cout << slice(test_vec, 0, -1) << std::endl;
    std::cout << slice(test_vec, 0, -2) << std::endl;
    std::cout << slice(test_vec, -3, -1) << std::endl;
    std::cout << test_vec << std::endl;

    std::vector<int> reversed(test_vec.size());
    std::reverse_copy(test_vec.begin(), test_vec.end(), reversed.begin());
    std::cout << reversed << std::endl;
    std::cout << std::make_tuple(1, 2) << std::endl;
    std::cout << zip(test_vec, reversed) << std::endl;

    // test the power of generics
    std::vector<std::string> str_vec{"this", "is", "a", "string", "vector", "cool"};
    auto sliced{slice(str_vec, -4, -1)};
    std::cout << sliced << std::endl;
    std::cout << zip(sliced, test_vec) << std::endl;

    std::cout << pairwise(test_vec) << std::endl;
}