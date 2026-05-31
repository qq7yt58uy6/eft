#include "injector.h"

#include <fstream>
#include <iostream>
#include <string>
#include <wbemcli.h>
#include <TlHelp32.h>
#include "../libs/ntdll.h"

#include "../util/globals.h"
#include "../util/logger.h"

using _NtQueryInformationProcess = NTSTATUS(NTAPI*)( // NOLINT(clang-diagnostic-reserved-identifier, bugprone-reserved-identifier)
    HANDLE ProcessHandle,
    DWORD ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
);

template <typename T>
T read(HANDLE h, PVOID address)
{
    T buffer = {};
    ReadProcessMemory(h, address, &buffer, sizeof(buffer), nullptr);

    return buffer;
}

template <typename T>
T* read(HANDLE h, PVOID address, size_t size)
{
    T* buffer[size];
    ReadProcessMemory(h, address, &buffer, size, nullptr);

    return buffer;
}

template <typename T>
void write(HANDLE h, PVOID address, const T& object)
{
    WriteProcessMemory(h, address, &object, sizeof(object), nullptr);
}

template <typename T>
void write(HANDLE h, PVOID address, const T* buffer, size_t size)
{
    WriteProcessMemory(h, address, buffer, size, nullptr);
}

// thanks archie
bool remove_module_lrd_entry(HANDLE hProcess, HMODULE hModule)
{
    auto NtQueryInformationProcess =
        reinterpret_cast<_NtQueryInformationProcess>(GetProcAddress(GetModuleHandleA("ntdll.dll"), // NOLINT(clang-diagnostic-cast-function-type-strict)
                                                                    "NtQueryInformationProcess"));

    if (!NtQueryInformationProcess)
    {
        return false;
    }

    PROCESS_BASIC_INFORMATION pbi;
    ULONG len;
    NTSTATUS status = NtQueryInformationProcess(hProcess, 0, &pbi, sizeof(pbi), &len);

    if (status != 0)
    {
        return false;
    }

    auto remote_ldr_ptr = read<PVOID>(
        hProcess, reinterpret_cast<PVOID>(reinterpret_cast<UINT64>(pbi.PebBaseAddress) + FIELD_OFFSET(PEB, Ldr))); // NOLINT(performance-no-int-to-ptr)
    auto remote_ldr = read<PEB_LDR_DATA>(hProcess, remote_ldr_ptr);


    // InLoadOrderModuleList
    do
    {
        const auto header_ptr = remote_ldr.InLoadOrderModuleList.Flink;
        auto current = read<LDR_DATA_TABLE_ENTRY>(hProcess, remote_ldr.InLoadOrderModuleList.Flink);
        do
        {
            if (!current.BaseDllName.MaximumLength)
            {
                current = read<LDR_DATA_TABLE_ENTRY>(hProcess, current.InLoadOrderLinks.Flink);
                continue;
            }

            uintptr_t entryBase = reinterpret_cast<uintptr_t>(current.DllBase);
            uintptr_t targetBase = reinterpret_cast<uintptr_t>(hModule);

            entryBase &= 0x00007FFFFFFFFFFF;
            targetBase &= 0x00007FFFFFFFFFFF;

            if (entryBase == targetBase)
            {
                logger::info("Found LDR Entry");

                // unlink entry
                logger::info("Unlinking LDR Entry");
                write(hProcess,
                      current.InLoadOrderLinks.Blink, current.InLoadOrderLinks.Flink);
                write(hProcess,
                      reinterpret_cast<PVOID>( // NOLINT(performance-no-int-to-ptr)
                          reinterpret_cast<UINT64>(current.InLoadOrderLinks.Flink) + sizeof(PVOID)),
                      current.InLoadOrderLinks.Blink);
                write(hProcess,
                      current.InMemoryOrderLinks.Blink, current.InMemoryOrderLinks.Flink);
                write(hProcess,
                      reinterpret_cast<PVOID>( // NOLINT(performance-no-int-to-ptr)
                          reinterpret_cast<UINT64>(current.InMemoryOrderLinks.Flink) + sizeof(PVOID)),
                      current.InMemoryOrderLinks.Blink);
                write(hProcess,
                      current.InInitializationOrderLinks.Blink, current.InInitializationOrderLinks.Flink);
                write(hProcess,
                      reinterpret_cast<PVOID>( // NOLINT(performance-no-int-to-ptr)
                          reinterpret_cast<UINT64>(current.InInitializationOrderLinks.Flink) + sizeof(PVOID)),
                      current.InInitializationOrderLinks.Blink);

                // change name of module
                logger::info("Changing LDR Entry Name");
                std::wstring short_name(L"X3DAudio1_7.dll");
                size_t length = short_name.length() * 2;
                write(hProcess, current.BaseDllName.Buffer, short_name.data(), length);
                write(
                    hProcess,
                    reinterpret_cast<PVOID>(reinterpret_cast<UINT64>(current.InLoadOrderLinks.Flink) + FIELD_OFFSET( // NOLINT(performance-no-int-to-ptr)
                        LDR_DATA_TABLE_ENTRY, BaseDllName.Length)), &length, sizeof(size_t));

                std::wstring long_name(L"C:\\Windows\\System32\\X3DAudio1_7.dll");
                length = long_name.length() * 2;
                write(hProcess, current.FullDllName.Buffer, long_name.data(), length);
                write(
                    hProcess,
                    reinterpret_cast<PVOID>(reinterpret_cast<UINT64>(current.InLoadOrderLinks.Flink) + FIELD_OFFSET( // NOLINT(performance-no-int-to-ptr)
                        LDR_DATA_TABLE_ENTRY, FullDllName.Length)), &length, sizeof(size_t));

                // change entry data
                logger::info("Changing LDR Entry Data");
                auto dll_base = reinterpret_cast<PVOID>(globals::int_rand(0, INT_MAX)); // NOLINT(performance-no-int-to-ptr)
                write(
                    hProcess,
                    reinterpret_cast<PVOID>(reinterpret_cast<UINT64>(current.InLoadOrderLinks.Flink) + FIELD_OFFSET( // NOLINT(performance-no-int-to-ptr)
                        LDR_DATA_TABLE_ENTRY, DllBase)), &dll_base, sizeof(PVOID));
                auto size_of_image = static_cast<ULONG>(globals::int_rand(0, INT_MAX)); // NOLINT(performance-no-int-to-ptr) 
                write(
                    hProcess,
                    reinterpret_cast<PVOID>(reinterpret_cast<UINT64>(current.InLoadOrderLinks.Flink) + FIELD_OFFSET( // NOLINT(performance-no-int-to-ptr)
                        LDR_DATA_TABLE_ENTRY, SizeOfImage)), &size_of_image, sizeof(ULONG));

                // strip headers
                logger::info("Stripping Headers");
                auto dos_header = read<IMAGE_DOS_HEADER>(hProcess, current.DllBase);
                auto pe_header = read<IMAGE_NT_HEADERS>(
                    hProcess, reinterpret_cast<PVOID>(reinterpret_cast<UINT64>(current.DllBase) + dos_header.e_lfanew)); // NOLINT(performance-no-int-to-ptr)

                size_t header_size = dos_header.e_lfanew +
                    offsetof(IMAGE_NT_HEADERS, OptionalHeader) + pe_header.FileHeader.
                                                                           SizeOfOptionalHeader
                    + pe_header.FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);

                auto garbage = static_cast<char*>(malloc(header_size));
                for (size_t i = 0; i < header_size; i++)
                {
                    garbage[i] = static_cast<char>(globals::int_rand(0, CHAR_MAX));
                }
                DWORD old_protect;
                VirtualProtectEx(hProcess, current.DllBase, header_size, PAGE_READWRITE, &old_protect);
                write(hProcess, current.DllBase, garbage);
                VirtualProtectEx(hProcess, current.DllBase, header_size, old_protect, nullptr);
                free(garbage);

                break;
            }

            if (current.InLoadOrderLinks.Flink == nullptr)
                break;

            current = read<LDR_DATA_TABLE_ENTRY>(hProcess, current.InLoadOrderLinks.Flink);
        }
        while (current.InLoadOrderLinks.Flink != header_ptr);
    }
    while (false);

    return true;
}

namespace injector
{
    bool should_inject = false;
    auto dlls = std::vector<std::string>();

    bool should_inject_res()
    {
        return should_inject;
    }

    std::vector<std::string> dlls_res()
    {
        return dlls;
    }

    void init()
    {
        std::ifstream file(globals::DLL_CONFIG_FILE);
        if (file)
        {
            std::string dllIn;
            std::string enabled_string;
            for (int i = 0; i < 2; ++i)
            {
                std::getline(file, dllIn);
            }
            for (int i = 0; i < 3; ++i)
            {
                std::getline(file, enabled_string);
            }
            dllIn = dllIn.substr(7);
            std::istringstream f(dllIn);
            std::string s;
            while (getline(f, s, ';'))
            {
                dlls.push_back(s);
            }

            enabled_string = enabled_string.substr(7);
            if (enabled_string == "true") should_inject = true;
        }
        else
        {
            return;
        }
        logger::info("Injector Enabled");
        file.close();
    }

    void inject(DWORD eft_pid, std::string dllIn)
    {
        logger::info("Injecting: " + dllIn);
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        auto wpath = std::wstring(path);
        size_t filename = wpath.find(L"BsgLauncher.exe");
        wpath = wpath.substr(0, filename);
        std::string dllInFull = std::string(wpath.begin(), wpath.end()).append(dllIn);
        auto dll = const_cast<char*>(dllInFull.c_str());
        HANDLE eft = OpenProcess(
            PROCESS_ALL_ACCESS, false, eft_pid);
        if (eft != nullptr)
        {
            auto fnLoadLibrary = reinterpret_cast<LPVOID>(GetProcAddress(
                GetModuleHandle(L"kernel32.dll"), "LoadLibraryA"));
            LPVOID mem = VirtualAllocEx(eft, nullptr, strlen(dll) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            WriteProcessMemory(eft, mem, dll, strlen(dll) + 1, nullptr);
            HANDLE thread = CreateRemoteThread(eft, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fnLoadLibrary),
                                               mem, 0, nullptr);
            while (WaitForSingleObject(thread, 0) != WAIT_OBJECT_0)
            {
                Sleep(10);
            }
            VirtualFreeEx(eft, mem, strlen(dll) + 1, MEM_RELEASE);

            // find module and remove ldr entry
            HMODULE found_module = nullptr;
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, eft_pid);
            if (snapshot != INVALID_HANDLE_VALUE)
            {
                MODULEENTRY32W me32;
                me32.dwSize = sizeof(MODULEENTRY32W);

                if (Module32FirstW(snapshot, &me32))
                {
                    do
                    {
                        if (std::wstring(me32.szModule) == std::wstring(dllIn.begin(), dllIn.end()))
                        {
                            found_module = reinterpret_cast<HMODULE>(me32.modBaseAddr);
                            break;
                        }
                    }
                    while (Module32NextW(snapshot, &me32));
                }
                CloseHandle(snapshot);
            }
            if (found_module)
            {
                remove_module_lrd_entry(eft, found_module);
                CloseHandle(found_module);
            }

            // confirm the module is hidden
            found_module = nullptr;
            snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, eft_pid);
            if (snapshot != INVALID_HANDLE_VALUE)
            {
                MODULEENTRY32W me32;
                me32.dwSize = sizeof(MODULEENTRY32W);

                if (Module32FirstW(snapshot, &me32))
                {
                    do
                    {
                        if (std::wstring(me32.szModule) == std::wstring(dllIn.begin(), dllIn.end()))
                        {
                            found_module = reinterpret_cast<HMODULE>(me32.modBaseAddr);
                            break;
                        }
                    }
                    while (Module32NextW(snapshot, &me32));
                }
                CloseHandle(snapshot);
            }
            if (found_module)
            {
                logger::warn("Module Hiding Failed");
            }
            else
            {
                logger::info("Confirmed Module Hidden");
            }

            CloseHandle(eft);
        }
    }
}
