#ifndef TESTLIB
#define TESTLIB

#include <functional>
#include <exception>
#include <optional>
#include <sstream>
#include <string>
#include "strlib.h"

namespace testutils
{
    template <typename T>
    std::string get_outstream(const T &value)
    {
        std::stringstream stream{};
        stream << value;
        return strlib::to_str(std::move(stream));
    }
}

namespace testlib
{
    template <typename EXCEPTION>
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
        assert(testutils::get_outstream(value) == expected_result);
    }
}

#endif
