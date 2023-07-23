#include <iostream>
#include <vector>
#include "itertools.h"

int max(std::vector<int> vector)
{
    int maximum = vector[0];
    for (int i = 1; i < vector.size(); i++)
    {
        maximum = std::max(maximum, vector[i]);
    }
    return maximum;
}

consteval int sum(const int lower_bound, const int upper_bound)
{
    int result{0};
    for (const int num : itertools::range(lower_bound, upper_bound))
        result += num;
    return result;
}

int main()
{
    std::vector<int> vect{10, 20, 30, 4};
    int value = max(vect);
    std::cout << value << std::endl;
    constexpr int gaussian_sum{sum(1, 101)};
    static_assert(gaussian_sum == 5050);
    std::cout << gaussian_sum << std::endl;
}