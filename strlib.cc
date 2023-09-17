#include <assert.h>
#include <string>
#include "strlib.h"
#include "ctest.h"

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
    ctest::assert_equal(result, "Hello there! General Kenobi!");

    std::string result2{strlib::format("Hello }", 1)};
    ctest::assert_equal(result2, "Hello }");

    std::string result3{strlib::format("{Hello", 1)};
    ctest::assert_equal(result3, "{Hello");

    // multiple brackets with only one arg shouldn't be filled
    std::string result4{strlib::format("Hello there! {} {} {}", "General Kenobi!")};
    ctest::assert_equal(result4, "Hello there! General Kenobi! {} {}");

    std::string result5{strlib::format("int: {} bool: {} string: {}", 1, true, "yes")};
    ctest::assert_equal(result5, "int: 1 bool: 1 string: yes");
}

int main()
{
    test_printable();
    test_format();
    std::cout << strlib::format("int: {} bool: {} string: {}", 1, true, "yes") << std::endl;
}
