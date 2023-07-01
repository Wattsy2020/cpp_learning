#include <iostream>
#include <vector>
#include <string>
#include <sstream>

std::string to_str(bool b)
{
    return (b) ? "true" : "false";
}

std::string to_str(std::vector<int> nums)
{
    std::stringstream result;
    result << "[ ";
    for (const int num : nums)
        result << num << " ";
    result << "]";
    return result.str();
}

std::ostream &operator<<(std::ostream &os, std::vector<int> vect)
{
    os << to_str(vect);
    return os;
}

bool is_sorted(std::vector<int> nums)
{
    for (size_t i = 1; i < nums.size(); i++)
    {
        if (nums[i] < nums[i - 1])
            return false;
    }
    return true;
}

void bubblesort(std::vector<int> &nums)
{
    int swaps;
    do
    {
        swaps = 0;
        for (size_t i = 0; i < nums.size() - 1; i++)
        {
            if (nums[i] > nums[i + 1])
            {
                std::swap(nums[i], nums[i + 1]);
                swaps++;
            }
        }
    } while (swaps > 0);
}

int main()
{
    std::vector<int> vect{1, 2, 3, 4, 5};
    std::cout << to_str(is_sorted(vect)) << std::endl;

    // swapping works
    std::swap(vect[0], vect[1]);
    std::cout << vect << std::endl;

    // check sorting
    std::vector<int> vect2{10, -5, 3, 4, 0};
    bubblesort(vect2);
    std::cout << vect2 << std::endl;
}