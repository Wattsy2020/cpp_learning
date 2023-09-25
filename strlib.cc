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
    // static_assert(!strlib::Printable<std::optional<std::vector<int>>>); // TODO: see if we can get nested printing working
}

void test_outstream()
{
    ctest::assert_outstream(std::vector<int>{1, 2, 3, 4, 5}, "[ 1 2 3 4 5 ]");
    ctest::assert_outstream(std::make_tuple<int, int>(1, 2), "(1, 2)");
    ctest::assert_outstream(std::optional<int>{}, "None");
    ctest::assert_outstream(std::optional<int>{2}, "2");
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

    std::cout << strlib::format("int: {} bool: {} string: {}", 1, true, "yes") << std::endl;
}

void test_split()
{
    std::string input{"Hello There! General Kenobi"};
    std::vector<std::string> expected{"Hello", "There!", "General", "Kenobi"};
    ctest::assert_equal(strlib::split(input), expected);

    ctest::assert_equal(strlib::split("Hello"), std::vector<std::string>{"Hello"});
}

int main()
{
    test_printable();
    test_outstream();
    test_format();
    test_split();
}
