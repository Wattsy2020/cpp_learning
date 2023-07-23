#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "itertools.h"
#include "strlib.h"

bool is_sorted(const std::vector<int> &nums)
{
    for (const auto [num, next_num] : itertools::pairwise(nums))
        if (num > next_num)
            return false;
    return true;
}

void bubblesort(std::vector<int> &nums)
{
    int swaps;
    do
    {
        swaps = 0;
        for (const auto [idx, data] : itertools::enumerate(itertools::pairwise(nums)))
        {
            const auto [num, num_next] = data;
            if (num > num_next)
            {
                std::swap(nums[idx], nums[idx + 1]);
                swaps++;
            }
        }
    } while (swaps > 0);
}

void _quicksort(std::vector<int> &nums, const int low, const int high)
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
            if (i != pivot_idx + 1)
                std::swap(nums[pivot_idx + 1], nums[i]);
            std::swap(nums[pivot_idx], nums[pivot_idx + 1]);
            pivot_idx++;
        }
    }

    // sort below and above the pivot_idx, the pivot_idx itself is already in sorted position
    // as it all smaller numbers are below it, and larger numbers above
    _quicksort(nums, low, pivot_idx);
    _quicksort(nums, pivot_idx + 1, high);
}

void quicksort(std::vector<int> &nums) { _quicksort(nums, 0, nums.size()); }

void test_quicksort()
{
    std::vector<int> vect3{4, 3, 5, 4};
    quicksort(vect3);
    assert(vect3 == (std::vector<int>{3, 4, 4, 5}));

    std::vector<int> vect4{10, -3, 100, 2, 0, 40, -1};
    quicksort(vect4);
    assert(vect4 == (std::vector<int>{-3, -1, 0, 2, 10, 40, 100}));

    std::vector<int> vect5{4, 4};
    quicksort(vect5);
    assert(vect5 == (std::vector<int>{4, 4}));
}

int main()
{
    std::vector<int> vect{1, 2, 3, 4, 5};
    std::cout << strlib::to_str(is_sorted(vect)) << std::endl;

    // check sorting
    std::vector<int> vect2{10, -5, 3, 4, 0};
    bubblesort(vect2);
    std::cout << vect2 << std::endl;

    // quicksort
    test_quicksort();
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