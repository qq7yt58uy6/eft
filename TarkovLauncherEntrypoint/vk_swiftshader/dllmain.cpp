#include <fstream>
#include <string>
#include <windows.h>

const std::string DLL_CONFIG_FILE = "launcher.cfg";

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            // resolve parent process id
            std::string args(GetCommandLineA());
            std::string sub("--host-process-id=");
            size_t start = args.find(sub);
            std::string cut_args = args.substr(start + sub.length());
            size_t end = cut_args.find(' ');
            std::string parent_string = cut_args.substr(0, end);
            DWORD parent_id = std::stoi(parent_string);
            
            // inject dll
            HANDLE parent = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, parent_id);
            if (parent != nullptr)
            {
                std::ifstream file(DLL_CONFIG_FILE);
                if (file)
                {
                    std::string dllIn;
                    std::getline(file, dllIn);
                    dllIn = dllIn.substr(7);
                    LPSTR dll = const_cast<char *>(dllIn.c_str());
                    file.close();
                    LPVOID fnLoadLibrary = reinterpret_cast<LPVOID>(GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA"));
                    LPVOID mem = VirtualAllocEx(parent, nullptr, strlen(dll) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); 
                    WriteProcessMemory(parent, mem, dll, strlen(dll) + 1, nullptr); 
                    HANDLE thread = CreateRemoteThread(parent, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fnLoadLibrary), mem, 0, nullptr);
                    while (WaitForSingleObject(thread, 0) != WAIT_OBJECT_0)
                    {
                        Sleep(10);
                    }
                    VirtualFreeEx(parent, mem, strlen(dll) + 1, MEM_RELEASE);
                    CloseHandle(parent);
                }
            }
        }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
	
    return TRUE;
}

