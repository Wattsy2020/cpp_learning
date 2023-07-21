#include <string>
#include "strlib.h"

std::string strlib::to_str(bool b)
{
    return (b) ? "true" : "false";
}