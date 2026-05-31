#pragma once
#include <string>
#include <strsafe.h>
#include <vector>

namespace injector
{
    bool should_inject_res();

    std::vector<std::string> dlls_res();

    void init();
    void inject(DWORD eft_pid, std::string dll);
};
