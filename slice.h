#ifndef ITERTOOLS_SLICE
#define ITERTOOLS_SLICE

#include <ranges>
#include <math.h>
#include <assert.h>

namespace __private_utils
{
    // Keep the idx within the bounds of 0, and the vector's length (for slicing only)
    // if idx is out of bounds, will bind it to the minimum
    constexpr int _bound_index(const long &idx, const long &length)
    {
        const long result = (idx < 0) ? std::max(length + idx, long{0}) : std::min(idx, length);
        assert(result >= 0 && result <= length);
        return result;
    }
}

namespace itertools
{
    // Slice an iterator from [start, end)  (i.e. not including the end index)
    // Handles negative numbers and slices where end > length
    template <std::ranges::input_range Iter>
    constexpr Iter slice(const Iter &iter, const long &start, const long &end)
    {
        const long length{std::distance(iter.begin(), iter.end())};
        const long start_idx{__private_utils::_bound_index(start, length)};
        const long end_idx{__private_utils::_bound_index(end, length)};
        return Iter(iter.begin() + start_idx, iter.begin() + end_idx);
    }
}

#endif
