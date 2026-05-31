#pragma once
#include <string>

namespace globals
{
    const std::string prod_name("PVE修改器");
    const std::string prod_ver("v1.0");
    const std::string credits("By: 如日中天");
    constexpr bool console = true;
    constexpr bool verbose = false;

    static std::string get_display_title()
    {
        return prod_name + " " + prod_ver;
    }
}
