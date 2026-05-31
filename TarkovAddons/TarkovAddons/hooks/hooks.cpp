#include "hooks.h"

#include <vector>

#include "../external/detours/detours.h"

namespace hooks
{
    namespace
    {
        auto hook_log = std::vector<hook_data>();
    }

    void hook(PVOID target, PVOID hook, PVOID* original)
    {
        *original = target;
        DetourTransactionBegin();               // 1. Begin transaction FIRST
        DetourUpdateThread(GetCurrentThread()); // 2. Update thread SECOND
        DetourAttach(original, hook);           // 3. Attach
        DetourTransactionCommit();              // 4. Commit
        hook_log.push_back({ hook, original });
    }

    void unhook(PVOID target, PVOID hook, PVOID* original)
    {
        *original = target;
        DetourTransactionBegin();               // 1. Begin transaction FIRST
        DetourUpdateThread(GetCurrentThread()); // 2. Update thread SECOND
        DetourDetach(original, hook);           // 3. Detach
        DetourTransactionCommit();              // 4. Commit
        int t = 0;
        for (int i = 0; i < hook_log.size(); ++i)
        {
            if (hook_log[i].original == original && hook_log[i].hook == hook)
            {
                t = i;
            }
        }
        hook_log.erase(hook_log.begin() + t);
    }

    void unhook_all()
    {
        for (auto log : hook_log)
        {
            DetourTransactionBegin();               // 1. Begin transaction FIRST
            DetourUpdateThread(GetCurrentThread()); // 2. Update thread SECOND
            DetourDetach(log.original, log.hook);   // 3. Detach
            DetourTransactionCommit();              // 4. Commit
        }
        hook_log.clear();
    }
}