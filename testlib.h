#include <functional>
#include <exception>

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
}