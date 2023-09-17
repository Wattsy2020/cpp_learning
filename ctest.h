#ifndef TESTLIB
#define TESTLIB

#include <functional>
#include <exception>
#include <optional>
#include <sstream>
#include <string>
#include <concepts>
#include "strlib.h"

namespace testutils
{
    template <typename T>
    std::string get_outstream(const T &value)
    {
        std::ostringstream stream{};
        stream << value;
        return stream.str();
    }
}

namespace ctest
{
    template <typename EXCEPTION = std::exception>
    void raises(const std::function<void()> &callback, std::optional<std::string> expected_message = std::nullopt)
    {
        try
        {
            callback();
        }
        catch (EXCEPTION &exc)
        {
            if (expected_message && expected_message.value() != exc.what())
                throw std::logic_error("The callback raised exception with an invalid message");
            return;
        }
        throw std::logic_error("The callback failed to raise the expected exception");
    }

    template <typename T>
    void assert_outstream(const T &value, std::string expected_result)
    {
        const std::string oustream_result{testutils::get_outstream(value)};
        assert(oustream_result == expected_result);
    }

    template <typename T1, typename T2>
        requires std::equality_comparable_with<T1, T2>
    void assert_equal(const T1 &left, const T2 &right)
    {
        if (left != right)
            throw std::logic_error(strlib::format("Error, operands are not equal\nLeft={}\nRight={}", left, right));
    }
}

#endif
