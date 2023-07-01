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

void _quicksort(std::vector<int> &nums, int low, int high)
{
    // split into two lists: one with elems less than pivot + pivot, second with elems >= pivot
    // then quicksort those lists
    // this is essential so that in the case with equal numbers e.g. [x, x], we split it into two lists [x] and [x]
    if (high - low <= 1)
        return;

    int pivot_idx = low;
    int pivot = nums[low];
    for (int i = low + 1; i < high; i++)
    {
        // if less than pivot, shift pivot up one and put nums[i] in its place
        if (nums[i] < pivot)
        {
            std::swap(nums[pivot_idx + 1], nums[i]);
            std::swap(nums[pivot_idx], nums[pivot_idx + 1]);
            pivot_idx++;
        }
    }

    _quicksort(nums, low, pivot_idx + 1);
    _quicksort(nums, pivot_idx + 1, high);
}

void quicksort(std::vector<int> &nums)
{
    _quicksort(nums, 0, nums.size());
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

    // quicksort
    std::vector<int> vect3{4, 3, 5, 4};
    std::vector<int> vect4{10, -3, 100, 2, 0, 40, -1};
    std::vector<int> vect5{4, 4};
    std::vector<std::vector<int>> testcases{vect3, vect4, vect5};
    for (std::vector<int> vec : testcases)
    {
        quicksort(vec);
        std::cout << vec << std::endl;
    }
}