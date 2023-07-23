#include <assert.h>
#include <string>
#include "strlib.h"

std::string strlib::to_str(bool b)
{
    return (b) ? "true" : "false";
}

std::string strlib::to_str(std::stringstream stream)
{
    return std::string(std::istreambuf_iterator(stream), {});
}

void test_format()
{
    std::string input{"Hello there! {}"};
    std::string result{strlib::format(input, "General Kenobi!")};
    assert(result == "Hello there! General Kenobi!");

    std::string result2{strlib::format("Hello }", 1)};
    assert(result2 == "Hello }");

    std::string result3{strlib::format("{Hello", 1)};
    assert(result3 == "{Hello");
}

// TODO: make this a test file once we have a testing framework
// int main() { test_format(); }
