#ifndef ITERTOOLS
#define ITERTOOLS

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <exception>
#include <optional>
#include <assert.h>
#include <ranges>
#include "math.h"
#include "strlib.h"
#include "slice.h"

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::tuple<T1, T2> &tuple)
{
    auto [first, second] = tuple;
    os << "(" << first << ", " << second << ")";
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &optional)
{
    if (optional)
        os << optional.value();
    else
        os << "None";
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vect)
{
    os << "[ ";
    for (const T v : vect)
        os << v << " ";
    os << "]";
    return os;
}

namespace __itertools_utils
{
    // Type erasure class that holds any iterator, satisfying an input_iterator
    // type erasure reference: https://www.modernescpp.com/index.php/type-erasure/
    template <typename T>
    class GenericIterator
    {
    public:
        typedef T value_type;
        typedef const value_type *pointer;
        typedef const value_type &reference;
        typedef std::ptrdiff_t difference_type;
        typedef std::input_iterator_tag iterator_category;

        GenericIterator() : iter_ptr{nullptr} {};

        // Implement copy constructors that deep copy the iterator, so we can call .begin() multiple times
        // Also implement move constructors to follow the rule of 5
        GenericIterator(const GenericIterator &other) : iter_ptr{other.clone()} {};

        GenericIterator &operator=(const GenericIterator &other)
        {
            GenericIterator(other).swap(*this);
            return *this;
        }

        GenericIterator(GenericIterator &&other)
        {
            other.swap(*this);
            other.iter_ptr = nullptr;
        }

        GenericIterator &operator=(GenericIterator &&other)
        {
            GenericIterator(std::move(other)).swap(*this);
            return *this;
        }

        // note: should have concepts like this: template <std::input_iterator Iter>  requires std::same_as<std::iter_value_t<Iter>, T>
        // but this doesn't work, because then determining whether GenericIterator itself satisfies std::input_iterator causes a recursion
        template <typename Iter>
        GenericIterator(const Iter &iter) : iter_ptr{std::make_unique<IteratorModel<Iter>>(IteratorModel<Iter>(iter))} {};

        // The abstract interface for an iterator
        struct IteratorConcept
        {
            virtual ~IteratorConcept(){};
            virtual reference operator*() const = 0;
            virtual IteratorConcept &operator++() = 0;
            virtual std::unique_ptr<IteratorConcept> clone() const = 0;
            virtual bool operator==(const IteratorConcept &) const = 0;
            virtual bool operator!=(const IteratorConcept &) const = 0;
        };

        // Take any iterator, and make a class for it that inherits from the abstract interface
        template <typename Iter>
        struct IteratorModel : IteratorConcept
        {
        public:
            IteratorModel(const Iter iter) : iter{iter} {}
            reference operator*() const override { return *iter; }
            IteratorModel &operator++() override
            {
                ++iter;
                return *this;
            }
            std::unique_ptr<IteratorConcept> clone() const override { return std::make_unique<IteratorModel<Iter>>(*this); };
            virtual bool operator==(const IteratorConcept &other) const override
            {
                auto other_ptr{dynamic_cast<const IteratorModel *>(&other)};
                return other_ptr != 0 && iter == other_ptr->iter;
            }
            virtual bool operator!=(const IteratorConcept &other) const override { return !(*this == other); }

        private:
            Iter iter;
        };

        // Finally, using the pointer to the abstract IteratorModel, we can implement the iterator methods
        reference operator*() const { return *(*iter_ptr); }
        GenericIterator<T> &operator++()
        {
            ++(*iter_ptr);
            return *this;
        }
        GenericIterator<T> operator++(int)
        {
            GenericIterator<T> temp(*this);
            ++*this;
            return temp;
        }
        std::unique_ptr<IteratorConcept> clone() const { return iter_ptr->clone(); }
        friend bool operator==(const GenericIterator &iter1, const GenericIterator &iter2) { return *iter1.iter_ptr == *iter2.iter_ptr; }
        friend bool operator!=(const GenericIterator &iter1, const GenericIterator &iter2) { return !(iter1 == iter2); }

    private:
        std::unique_ptr<IteratorConcept> iter_ptr;

        void swap(GenericIterator &other) noexcept { std::swap(this->iter_ptr, other.iter_ptr); }
    };

    // Type erasure class that holds an Range and provides an input iterator over it
    template <typename T>
    class GenericRange
    {
    public:
        template <std::ranges::input_range Range>
            requires std::same_as<std::iter_value_t<Range>, T>
        GenericRange(const Range &range) : begin_iter{GenericIterator<T>(range.begin())},
                                           end_iter{GenericIterator<T>(range.end())} {};

        GenericIterator<T> begin() const { return begin_iter; }

        GenericIterator<T> end() const { return end_iter; }

    private:
        GenericIterator<T> begin_iter;
        GenericIterator<T> end_iter;
    };

    /*
    // Class that holds several ranges and provides an input iterator to go over all of them
    template <std::ranges::input_range Range, std::ranges::input_range... Ranges>
    class Chain
    {
    public:
        Chain(Range range, Ranges... ranges) : range{range}, next_chain{std::nullopt}
        {
            if constexpr (sizeof...(ranges) > 0)
                next_chain = Chain(ranges...);
        }

    private:
        Range range;
        std::optional<Chain<Ranges...>> next_chain;
    };
    */
}

namespace itertools
{
    constexpr void validate_index(const int &index, const int &length)
    {
        if (index < 0 || index >= length)
            throw std::range_error(strlib::format("Invalid index {}, must be between 0 and {}", index, length));
    }

    template <std::ranges::bidirectional_range Range>
    constexpr Range reversed(const Range &range)
    {
        return Range(range.rbegin(), range.rend());
    }

    // A vector with items from [start, end) with the given step size
    constexpr std::vector<int> range(const int &start, const int &end, const int &step = 1)
    {
        if (step == 0)
        {
            throw std::invalid_argument("step cannot be 0");
        }
        else if (step > 0)
        {
            if (start > end)
                throw std::invalid_argument("need negative step when start > end");
        }
        else if (start < end)
        {
            throw std::invalid_argument("need positive step when start < end");
        }

        std::vector<int> result{};
        const int sign{signum(end - start)};
        for (int i{start}; sign * i < sign * end; i = i + step)
            result.push_back(i);
        return result;
    }

    // Zip two ranges together, returning a vector where vec[i] = tuple{iter1[i], iter2[i]}
    // Stop at the end of the shortest range
    template <std::ranges::input_range Range1, std::ranges::input_range Range2>
    constexpr std::vector<std::tuple<std::iter_value_t<Range1>, std::iter_value_t<Range2>>> zip(const Range1 &range1, const Range2 &range2)
    {
        typename Range1::const_iterator begin1{range1.begin()};
        const typename Range1::const_iterator end1{range1.end()};
        typename Range2::const_iterator begin2{range2.begin()};
        const typename Range2::const_iterator end2{range2.end()};
        std::vector<std::tuple<std::iter_value_t<Range1>, std::iter_value_t<Range2>>> result{};
        while (begin1 != end1 && begin2 != end2)
        {
            result.emplace_back(*begin1, *begin2);
            ++begin1;
            ++begin2;
        }
        return result;
    }

    // Return tuple of i and the vector item at i
    template <std::ranges::input_range Range>
    constexpr std::vector<std::tuple<int, std::iter_value_t<Range>>> enumerate(const Range &range)
    {
        return itertools::zip(itertools::range(0, range.size()), range);
    }

    // Return vector of tuples (item, next item)
    template <std::ranges::input_range Range>
    constexpr std::vector<std::tuple<std::iter_value_t<Range>, std::iter_value_t<Range>>> pairwise(const Range &range)
    {

        return itertools::zip(range, itertools::slice(range, 1));
    }

    // Return tuple (first part of vector, last element of vector)
    // Note the vector must contain at least one element
    template <typename T>
    constexpr std::tuple<std::vector<T>, T> init_last(const std::vector<T> &vec)
    {
        if (vec.empty())
            throw std::length_error("Vector must have at least one element");
        return std::make_tuple(itertools::slice(vec, 0, -1), vec[vec.size() - 1]);
    }

    // Return tuple (first element of vector, last part of vector)
    // Vector must contain at least one element
    template <typename T>
    constexpr std::tuple<T, std::vector<T>> head_tail(const std::vector<T> &vec)
    {
        if (vec.empty())
            throw std::length_error("Vector must have at least one element");
        return std::make_tuple(vec[0], itertools::slice(vec, 1));
    }

    // Join elements of an iterator together, using the separator
    template <std::ranges::input_range Range>
    std::string join(const Range &range, const std::string &separator)
    {
        typename Range::const_iterator begin_iter{range.begin()};
        const typename Range::const_iterator end_iter{range.end()};
        if (begin_iter == end_iter)
            return "";

        std::ostringstream stream{};
        stream << *begin_iter++;
        for (; begin_iter != end_iter; ++begin_iter)
            stream << separator << *begin_iter;
        return stream.str();
    }

    // Chain any number of iterators together
    template <std::ranges::input_range Range, std::ranges::input_range... Ranges>
    void _chain_accumulator(std::vector<std::iter_value_t<Range>> &accumulator_vec, const Range &range, const Ranges... ranges)
    {
        std::copy(range.begin(), range.end(), std::back_inserter(accumulator_vec));
        if constexpr (sizeof...(ranges) > 0)
            _chain_accumulator(accumulator_vec, ranges...);
    }

    // Chain any number of iterators together
    template <std::ranges::input_range Range, std::ranges::input_range... Ranges>
    constexpr std::vector<std::iter_value_t<Range>> chain(const Range &range, const Ranges... ranges)
    {
        std::vector<std::iter_value_t<Range>> combined(range.begin(), range.end());
        if constexpr (sizeof...(ranges) > 0)
            _chain_accumulator(combined, ranges...);
        return combined;
    }
}

#endif
