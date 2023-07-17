#include "functools.h"

void test_any_all()
{
    std::vector<bool> bools{true, true, false, false};
    assert(any(bools) == true);
    assert(all(bools) == false);
    assert(all(std::vector<bool>{true, true}) == true);
}

void test_map()
{
    std::vector<int> nums = range(1, 8, 2);
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::vector<bool> result{map(greater_than_3, nums)};
    std::vector<bool> expected_result{false, false, true, true};
    assert(result == expected_result);
}

void test_compose()
{
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::function<std::string(int)> composed{compose(greater_than_3, std::function<std::string(bool)>(to_str))};
    std::string expected_result{"true"};
    std::string result{composed(5)};
    assert(result == expected_result);
}

void test_count()
{
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::vector<int> nums = range(1, 8, 2);
    int result{count(greater_than_3, nums)};
    assert(result == 2);
}

void test_sum()
{
    int result{sum(std::vector<int>{0, 1, 2, 3, 4})};
    assert(result == 10);
    double result2{sum(std::vector<double>{-1.333, 1.333, 2.1})};
    assert(result2 - 2.1 < .00001);
}

int main()
{
    test_any_all();
    test_map();
    test_compose();
    test_count();
    test_sum();

    std::vector<bool> bools{true, true, false, false};
    std::cout << to_str(any(bools)) << " " << to_str(all(bools)) << " for " << bools << std::endl;

    std::vector<int> nums = range(1, 20, 2);
    std::function<bool(int)> greater_than_3{[](int a)
                                            { return a > 3; }};
    std::cout << nums << " " << map(compose(greater_than_3, std::function<std::string(bool)>(to_str)), nums) << std::endl;
    std::cout << count(greater_than_3, nums) << std::endl;

    std::cout << sum(std::vector<int>{0, 1, 2, 3, 4}) << std::endl;
    std::cout << sum(std::vector<double>{-1.333, 1.333, 2.1}) << std::endl;

    std::cout << to_str(all(greater_than_3, nums)) << std::endl;
    std::cout << to_str(any(greater_than_3, nums)) << std::endl;
}