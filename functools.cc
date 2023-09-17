#include <assert.h>
#include <string>
#include "functools.h"
#include "itertools.h"
#include "strlib.h"
#include "ctest.h"

// unfortunately have to static cast overloaded functions in order to construct a std::function
std::function<std::string(bool)> bool_to_str{static_cast<std::string (*)(bool)>(&strlib::to_str)};

void test_any_all()
{
    std::vector<bool> bools{true, true, false, false};
    ctest::assert_equal(functools::any(bools), true);
    ctest::assert_equal(functools::all(bools), false);
    ctest::assert_equal(functools::all(std::vector<bool>{true, true}), true);
}

void test_map()
{
    std::vector<int> nums = itertools::range(1, 8, 2);
    std::vector<bool> result{functools::map([](int a)
                                            { return a > 3; },
                                            nums)};
    std::vector<bool> expected_result{false, false, true, true};
    ctest::assert_equal(result, expected_result);
}

void test_filter()
{
    std::vector<int> nums = itertools::range(1, 8, 2);
    auto greater_than_3{[](int a)
                        { return a > 3; }};
    std::vector<int> result{functools::filter(greater_than_3, nums)};
    std::vector<int> expected_result{5, 7};
    ctest::assert_equal(result, expected_result);
}

void test_compose()
{
    auto greater_than_3{[](int a)
                        { return a > 3; }};
    std::function<std::string(int)> composed{functools::compose<int>(greater_than_3, bool_to_str)};
    std::string expected_result{"true"};
    std::string result{composed(5)};
    ctest::assert_equal(result, expected_result);
}

void test_count()
{
    auto greater_than_3{[](int a)
                        { return a > 3; }};
    std::vector<int> nums = itertools::range(1, 8, 2);
    int result{functools::count(greater_than_3, nums)};
    ctest::assert_equal(result, 2);
}

void test_sum()
{
    int result{functools::sum(std::vector<int>{0, 1, 2, 3, 4})};
    ctest::assert_equal(result, 10);
    double result2{functools::sum(std::vector<double>{-1.333, 1.333, 2.1})};
    assert(result2 - 2.1 < .00001);
}

bool greater_than(const int compare_to, const int num) { return num > compare_to; }

void test_partial()
{
    std::function<bool(int, int)> greater_than_func{greater_than};
    assert(functools::Partial(greater_than_func, 3)(5));
    assert(!functools::Partial(greater_than_func, 10)(7));
}

void test_optional()
{
    std::optional<int> value{1};
    auto add_one{[](int x)
                 { return x + 1; }};
    ctest::assert_equal(functools::transform(add_one, value), std::optional<int>{2});
    std::optional<int> empty_optional{};
    assert(!functools::transform(add_one, empty_optional).has_value());
}

void test_for_each()
{
    std::vector<int> values{1, 2, 3, 4};
    auto add_one{[](int &x)
                 { ++x; }};
    functools::for_each(add_one, values);
    ctest::assert_equal(values, std::vector<int>{2, 3, 4, 5});
}

int main()
{
    test_any_all();
    test_map();
    test_filter();
    test_compose();
    test_count();
    test_sum();
    test_partial();
    test_optional();
    test_for_each();

    std::vector<bool> bools{true, true, false, false};
    std::cout << strlib::to_str(functools::any(bools)) << " " << strlib::to_str(functools::all(bools)) << " for " << bools << std::endl;

    std::vector<int> nums = itertools::range(1, 20, 2);
    std::function<bool(int, int)> greater_than_func{greater_than};
    std::function<bool(int)> greater_than_3{functools::Partial(greater_than_func, 3)};
    std::cout << nums << " " << functools::map(functools::compose<int>(greater_than_3, bool_to_str), nums) << std::endl;
    std::cout << functools::count(greater_than_3, nums) << std::endl;
    std::cout << functools::count(to_func(functools::Partial(greater_than_func, 10)), nums) << std::endl;

    std::cout << functools::sum(std::vector<int>{0, 1, 2, 3, 4}) << std::endl;
    std::cout << functools::sum(std::vector<double>{-1.333, 1.333, 2.1}) << std::endl;

    std::cout << strlib::to_str(functools::all(greater_than_3, nums)) << std::endl;
    std::cout << strlib::to_str(functools::any(greater_than_3, nums)) << std::endl;
}