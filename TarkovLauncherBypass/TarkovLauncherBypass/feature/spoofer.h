//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Spoofer is a cleaned up version of HWho by hollow
/// https://www.unknowncheats.me/forum/escape-from-tarkov/494040-hwho-slightly-fun-bsg-launcher-hwid-check-bypass.html
/// Credit goes to all original contributors.
/// 

#pragma once
#include <strsafe.h>

namespace spoofer
{
    void init();
    void game(DWORD eft_pid);
};
