#ifndef ITERTOOLS_SLICE
#define ITERTOOLS_SLICE

#include <ranges>
#include <optional>
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
    // Slice a range from [start, end)  (i.e. not including the end index)
    // Handles negative numbers and slices where end > length
    // Call without end to include all elements in the range after start
    template <std::ranges::input_range Range>
    constexpr std::ranges::view auto slice(const Range &range, const long &start, const std::optional<long> &end = std::nullopt)
    {
        auto begin_iter{range.begin()};
        auto end_iter{range.end()};
        const long length{std::distance(begin_iter, end_iter)};
        const long start_idx{__private_utils::_bound_index(start, length)};
        const long end_idx{(end) ? __private_utils::_bound_index(end.value(), length) : length};
        return range | std::views::drop(start_idx) | std::views::take(end_idx - start_idx);
    }
}

#endif
