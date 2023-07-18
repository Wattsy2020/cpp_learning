#ifndef TESTLIB
#define TESTLIB

#include <functional>
#include <exception>
#include <sstream>

namespace testutils
{
    template <typename T>
    std::string get_outstream(const T &value)
    {
        std::stringstream stream{};
        stream << value;
        return std::string(std::istreambuf_iterator(stream), {});
    }
}

namespace testlib
{
    template <typename EXCEPTION>
    void raises(const std::function<void()> &callback)
    {
        try
        {
            callback();
        }
        catch (EXCEPTION)
        {
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
