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

struct test_struct
{
    int a;
};

void test_printable()
{
    static_assert(strlib::Printable<std::string>);
    static_assert(strlib::Printable<char>);
    static_assert(strlib::Printable<int>);
    static_assert(strlib::Printable<bool>);
    static_assert(!strlib::Printable<test_struct>);
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

    // multiple brackets with only one arg shouldn't be filled
    std::string result4{strlib::format("Hello there! {} {} {}", "General Kenobi!")};
    assert(result4 == "Hello there! General Kenobi! {} {}");

    std::string result5{strlib::format("int: {} bool: {} string: {}", 1, true, "yes")};
    assert(result5 == "int: 1 bool: 1 string: yes");
}

// TODO: make this a test file once we have a testing framework
/*
int main()
{
    test_printable();
    test_format();
    std::cout << strlib::format("int: {} bool: {} string: {}", 1, true, "yes") << std::endl;
}
*/
