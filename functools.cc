#include <string>
#include "functools.h"
#include "itertools.h"
#include "string_lib.h"

void test_any_all()
{
    std::vector<bool> bools{true, true, false, false};
    assert(functools::any(bools) == true);
    assert(functools::all(bools) == false);
    assert(functools::all(std::vector<bool>{true, true}) == true);
}

void test_map()
{
    std::vector<int> nums = itertools::range(1, 8, 2);
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::vector<bool> result{functools::map(greater_than_3, nums)};
    std::vector<bool> expected_result{false, false, true, true};
    assert(result == expected_result);
}

void test_filter()
{
    std::vector<int> nums = itertools::range(1, 8, 2);
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::vector<int> result{functools::filter(greater_than_3, nums)};
    std::vector<int> expected_result{5, 7};
    assert(result == expected_result);
}

void test_compose()
{
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::function<std::string(int)> composed{functools::compose(greater_than_3, std::function<std::string(bool)>(to_str))};
    std::string expected_result{"true"};
    std::string result{composed(5)};
    assert(result == expected_result);
}

void test_count()
{
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::vector<int> nums = itertools::range(1, 8, 2);
    int result{functools::count(greater_than_3, nums)};
    assert(result == 2);
}

void test_sum()
{
    int result{functools::sum(std::vector<int>{0, 1, 2, 3, 4})};
    assert(result == 10);
    double result2{functools::sum(std::vector<double>{-1.333, 1.333, 2.1})};
    assert(result2 - 2.1 < .00001);
}

int main()
{
    test_any_all();
    test_map();
    test_filter();
    test_compose();
    test_count();
    test_sum();

    std::vector<bool> bools{true, true, false, false};
    std::cout << to_str(functools::any(bools)) << " " << to_str(functools::all(bools)) << " for " << bools << std::endl;

    std::vector<int> nums = itertools::range(1, 20, 2);
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::cout << nums << " " << functools::map(functools::compose(greater_than_3, std::function<std::string(bool)>(to_str)), nums) << std::endl;
    std::cout << functools::count(greater_than_3, nums) << std::endl;

    std::cout << functools::sum(std::vector<int>{0, 1, 2, 3, 4}) << std::endl;
    std::cout << functools::sum(std::vector<double>{-1.333, 1.333, 2.1}) << std::endl;

    std::cout << to_str(functools::all(greater_than_3, nums)) << std::endl;
    std::cout << to_str(functools::any(greater_than_3, nums)) << std::endl;
}