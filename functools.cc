#include <vector>
#include <iostream>
#include "itertools.h"
#include "string_lib.h"

constexpr bool any(const std::vector<bool> &bools)
{
    for (bool value : bools)
        if (value)
            return true;
    return false;
}

constexpr bool all(const std::vector<bool> &bools)
{
    for (bool value : bools)
        if (!value)
            return false;
    return true;
}

int main()
{
    std::vector<bool> bools{true, true, false, false};
    std::cout << to_str(any(bools)) << " " << to_str(all(bools)) << " for " << bools << std::endl;
    std::cout << to_str(all(std::vector<bool>{true, true, true})) << std::endl;
}