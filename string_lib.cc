#include <string>
#include "string_lib.h"

std::string strlib::to_str(bool b)
{
    return (b) ? "true" : "false";
}