#include "math.h"

// return 1 for positive x, 0 for 0, -1 for negative x
int signum(int x)
{
    if (x > 0)
        return 1;
    if (x == 0)
        return 0;
    return -1;
}