#pragma once
#include <Windows.h>

namespace hooks
{
    void hook(PVOID target, PVOID hook, PVOID* original);

    void unhook(PVOID target, PVOID hook, PVOID* original);

    void unhook_all();

    struct hook_data
    {
        PVOID hook;
        PVOID* original;
    };
}
