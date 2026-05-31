//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Launcher spoofer is a cleaned up version of HWho by hollow
/// https://www.unknowncheats.me/forum/escape-from-tarkov/494040-hwho-slightly-fun-bsg-launcher-hwid-check-bypass.html
/// Credit goes to all original contributors.
/// 

#include "spoofer.h"

#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <windows.h>
#include <wbemcli.h>
#include <sstream>
#include <TlHelp32.h>

#include "../libs/detours.h"
#include "../util/globals.h"
#include "../util/logger.h"

struct RawSMBIOSData
{
    BYTE Used20CallingMethod;
    BYTE SMBIOSMajorVersion;
    BYTE SMBIOSMinorVersion;
    BYTE DmiRevision;
    DWORD Length;
    BYTE SMBIOSTableData[]; // NOLINT(clang-diagnostic-c99-extensions)
};

struct RawSMBIOSTable
{
    BYTE Type;
    BYTE Length;
    WORD Handle;
};

struct RawBiosInformationTable : RawSMBIOSTable
{
    BYTE Vendor;
    BYTE BiosVersion;
    UINT16 BiosSegment;
    BYTE BiosReleaseDate;
    UINT8 BiosSize;
};

struct RawSystemInformationTable : RawSMBIOSTable
{
    BYTE Manufacturer;
    BYTE ProductName;
    BYTE Version;
    BYTE SerialNumber;
    GUID Uuid;
    UINT8 WakeUpType;
    BYTE SKUNumber;
    BYTE Family;
};

struct RawBaseboardInformationTable : RawSMBIOSTable
{
    BYTE Manufacturer;
    BYTE ProductName;
    BYTE Version;
    BYTE SerialNumber;
};

struct RawProcessorInformationTable : RawSMBIOSTable
{
    BYTE Socket;
    UINT8 ProcessorType;
    UINT8 ProcessorFamily;
    BYTE ProcessorManufacturer;
    BYTE ProcessorId[8];
    BYTE ProcessorVersion;
    BYTE Voltage;
    UINT16 ExternalClock;
    UINT16 MaxSpeed;
    UINT16 CurrentSpeed;
    UINT8 Status;
    UINT8 ProcessorUpgrade;
    UINT16 L1CacheHandle;
    UINT16 L2CacheHandle;
    UINT16 L3CacheHandle;
    BYTE SerialNumber;
    BYTE AssetTag;
    BYTE PartNumber;
};

static std::vector<unsigned char> key = {};

static PSTR get_sm_bios_string(RawSMBIOSTable* SmBiosTable, BYTE StringIndex)
{
    BYTE CurrentStringIndex = 1;
    auto currentString = reinterpret_cast<PSTR>(reinterpret_cast<BYTE*>(SmBiosTable) + SmBiosTable->Length);

    while (*currentString)
    {
        if (CurrentStringIndex == StringIndex)
        {
            break;
        }

        ++currentString;

        if (!*currentString)
        {
            ++currentString;
            ++CurrentStringIndex;
        }
    }

    if (!*currentString)
        return nullptr;

    return currentString;
}

static void mangle_sm_bios_string(RawSMBIOSTable* SmBiosTable, BYTE StringIndex)
{
    char* str = get_sm_bios_string(SmBiosTable, StringIndex);
    if (!str)
    {
        return;
    }

    const auto length = strlen(str);
    for (size_t i = 0; i < length; i++)
    {
        const auto original_character = str[i];
        auto rng = original_character ^ key[i % 8];

        if (std::isdigit(original_character))
            rng = static_cast<unsigned>(rng) % 10 + 48; // NOLINT(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
        else
            continue;

        rng &= 0x7F; // remove extended ascii
        str[i] = static_cast<char>(rng);
    }
}

UINT (*oGetSystemFirmwareTable)(DWORD, DWORD, PVOID, DWORD) = nullptr;

UINT __stdcall hkGetSystemFirmwareTable(DWORD FirmwareTableProviderSignature, DWORD FirmwareTableID,
                                        PVOID pFirmwareTableBuffer, DWORD BufferSize)
{
    UINT result = oGetSystemFirmwareTable(FirmwareTableProviderSignature, FirmwareTableID, pFirmwareTableBuffer,
                                          BufferSize);

    if (FirmwareTableProviderSignature == 'RSMB' && FirmwareTableID == 0 && pFirmwareTableBuffer != nullptr)
    {
        auto SMBIOSData = static_cast<RawSMBIOSData*>(pFirmwareTableBuffer);
        RawSMBIOSTable* smbiosTable = nullptr;
        ULONG i = 0;

        bool properTermination;

        do
        {
            properTermination = false;

            // Check that the table header fits in the buffer.
            if (i + sizeof(RawSMBIOSTable) < SMBIOSData->Length)
            {
                ULONG type = SMBIOSData->SMBIOSTableData[i];

                if (type <= 4)
                    smbiosTable = reinterpret_cast<RawSMBIOSTable*>(&SMBIOSData->SMBIOSTableData[i]);

                // Set i to the end of the formated section.
                i += SMBIOSData->SMBIOSTableData[i + 1];

                // Look for the end of the struct that must be terminated by \0\0
                while (i + 1 < SMBIOSData->Length)
                {
                    if (0 == SMBIOSData->SMBIOSTableData[i] &&
                        0 == SMBIOSData->SMBIOSTableData[i + 1])
                    {
                        properTermination = true;
                        i += 2;
                        break;
                    }

                    ++i;
                }

                if (properTermination && smbiosTable)
                {
                    switch (type)
                    {
                    case 0:
                        {
                            auto biosInfo = static_cast<RawBiosInformationTable*>(smbiosTable);
                            mangle_sm_bios_string(smbiosTable, biosInfo->Vendor);
                            mangle_sm_bios_string(smbiosTable, biosInfo->BiosVersion);
                            mangle_sm_bios_string(smbiosTable, biosInfo->BiosReleaseDate);
                        }
                        break;

                    case 1:
                        {
                            auto systemInfo = static_cast<RawSystemInformationTable*>(smbiosTable);

                            mangle_sm_bios_string(smbiosTable, systemInfo->Manufacturer);
                            mangle_sm_bios_string(smbiosTable, systemInfo->ProductName);
                            mangle_sm_bios_string(smbiosTable, systemInfo->Version);
                            mangle_sm_bios_string(smbiosTable, systemInfo->SerialNumber);

                            if (systemInfo->Length > 25)
                            {
                                mangle_sm_bios_string(smbiosTable, systemInfo->SKUNumber);
                                mangle_sm_bios_string(smbiosTable, systemInfo->Family);
                            }

                            auto guidPtr = reinterpret_cast<char*>(&systemInfo->Uuid);
                            for (size_t j = 0; j < sizeof(GUID); j++)
                                guidPtr[j] ^= key[j % 8]; // NOLINT(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
                        }
                        break;

                    case 2:
                        {
                            auto baseboardInfo = static_cast<RawBaseboardInformationTable*>(smbiosTable);

                            mangle_sm_bios_string(smbiosTable, baseboardInfo->Manufacturer);
                            mangle_sm_bios_string(smbiosTable, baseboardInfo->ProductName);
                            mangle_sm_bios_string(smbiosTable, baseboardInfo->Version);
                            mangle_sm_bios_string(smbiosTable, baseboardInfo->SerialNumber);
                        }
                        break;

                    case 4:
                        {
                            auto processorInfo = static_cast<RawProcessorInformationTable*>(smbiosTable);

                            mangle_sm_bios_string(smbiosTable, processorInfo->ProcessorManufacturer);
                            mangle_sm_bios_string(smbiosTable, processorInfo->ProcessorVersion);
                            if (processorInfo->Length > 32)
                            {
                                mangle_sm_bios_string(smbiosTable, processorInfo->SerialNumber);
                                mangle_sm_bios_string(smbiosTable, processorInfo->PartNumber);
                            }

                            for (int j = 0; j < 4; j++) // PROCESSOR_SIGNATURE
                                processorInfo->ProcessorId[j] ^= key[j % 8];
                        }
                        break;

                    default:
                        break;
                    }

                    smbiosTable = nullptr;
                }
            }
        }
        while (properTermination);
    }

    return result;
}


HRESULT (*oGetFn)(IWbemClassObject*, LPCWSTR, LONG, VARIANT*, CIMTYPE*, long*) = nullptr;

HRESULT __stdcall hkGetFn(IWbemClassObject* pThis, LPCWSTR wszName, LONG lFlags, VARIANT* pVal, CIMTYPE* pType,
                          long* plFlavor)
{
    const auto original_result = oGetFn(pThis, wszName, lFlags, pVal, pType, plFlavor);

    // only process strings
    if (pVal->vt != VT_BSTR)
        return original_result;

    // ignored values
    if (lstrcmpW(wszName, L"__GENUS") == 0 || lstrcmpW(wszName, L"__PATH") == 0 || lstrcmpW(wszName, L"__RELPATH") == 0)
    {
        return original_result;
    }

    const size_t length = lstrlenW(pVal->bstrVal);
    for (size_t i = 0; i < length; i++)
    {
        const auto original_character = static_cast<char>(pVal->bstrVal[i]);
        auto rng = original_character ^ key[i % 8];

        if (std::isdigit(original_character))
            rng = static_cast<unsigned>(rng) % 10 + 48; // NOLINT(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
        else
            continue;

        rng &= 0x7F; // remove extended ascii
        pVal->bstrVal[i] = static_cast<WCHAR>(rng);
    }

    return original_result;
}

namespace spoofer
{
    static bool should_spoof = false;

    const std::string HWID_FILE = "hwid.bin";
    const std::string GAMEASSEMBLY = "GameAssembly.dll";
    // search HWEcho_Json -> xref the function its in, sig the function call above that which is the check
    const std::string NEEDS_HWID_CHECK_SIG =
        "E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 33 D2 48 8D 4C 24 ? E8 ? ? ? ? ? ? ? 0F 11 44 24";
    constexpr size_t HWID_FUNCTION_PATCH_SIZE = 5;

    void init()
    {
        std::ifstream config_file(globals::DLL_CONFIG_FILE);
        if (config_file)
        {
            std::string enabled_string;
            for (int i = 0; i < 3; ++i)
            {
                std::getline(config_file, enabled_string);
            }
            enabled_string = enabled_string.substr(8);
            if (enabled_string != "true") return;
        }
        else
        {
            return;
        }
        logger::info("Spoofer Enabled");
        should_spoof = true;
        config_file.close();

        std::basic_ifstream<unsigned char> file(HWID_FILE, std::ios::binary);
        if (file)
        {
            key = std::vector<unsigned char>((std::istreambuf_iterator<unsigned char>(file)),
                                             std::istreambuf_iterator<unsigned char>());
            logger::info("Loading HWID Key");
            std::stringstream ss;
            for (size_t i = 0; i < 12; i++)
                ss << std::hex << (key[i] & 0xFF);
            logger::info("Key: " + ss.str());
        }
        else
        {
            logger::info("Generating HWID Key");
            std::stringstream ss;
            for (size_t i = 0; i < 12; i++)
            {
                key.emplace_back(globals::int_rand(0, 0xFF));
                ss << std::hex << (key[i] & 0xFF);
            }
            logger::info("Key: " + ss.str());

            std::ofstream fout(HWID_FILE, std::ios::out | std::ios::binary);
            fout.write(reinterpret_cast<char*>(key.data()), static_cast<std::streamsize>(key.size()));
        }

        HMODULE fastprox = nullptr;
        while (fastprox == nullptr)
        {
            fastprox = GetModuleHandleA("fastprox.dll");
            Sleep(100);
        }

        const auto getFn = GetProcAddress(fastprox, "?Get@CWbemObject@@UEAAJPEBGJPEAUtagVARIANT@@PEAJ2@Z");
        if (getFn == nullptr)
        {
            logger::error("Failed to find get function!");
            Sleep(2000);
            exit(0); // NOLINT(concurrency-mt-unsafe)
        }

        const auto getSystemFirmwareTable =
            GetProcAddress(GetModuleHandleA("KernelBase.dll"), "GetSystemFirmwareTable");
        if (getSystemFirmwareTable == nullptr)
        {
            logger::error("Failed to find firmware function!");
            Sleep(2000);
            exit(0); // NOLINT(concurrency-mt-unsafe)
        }

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        oGetFn = reinterpret_cast<HRESULT(*)(IWbemClassObject*, LPCWSTR, LONG, VARIANT*, CIMTYPE*, long*)>(getFn); // NOLINT(clang-diagnostic-cast-function-type-strict, clang-diagnostic-undefined-reinterpret-cast)
        DetourAttach(&reinterpret_cast<PVOID&>(oGetFn), &hkGetFn); // NOLINT(clang-diagnostic-microsoft-cast, clang-diagnostic-undefined-reinterpret-cast)
        if (DetourTransactionCommit() != NO_ERROR)
        {
            logger::error("Failed to hook get function!");
            Sleep(2000);
            exit(0); // NOLINT(concurrency-mt-unsafe)
        }

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        oGetSystemFirmwareTable = reinterpret_cast<UINT(*)(DWORD, DWORD, PVOID, DWORD)>(getSystemFirmwareTable); // NOLINT(clang-diagnostic-cast-function-type-strict, clang-diagnostic-undefined-reinterpret-cast)
        DetourAttach(&reinterpret_cast<PVOID&>(oGetSystemFirmwareTable), &hkGetSystemFirmwareTable); // NOLINT(clang-diagnostic-microsoft-cast, clang-diagnostic-undefined-reinterpret-cast)
        if (DetourTransactionCommit() != NO_ERROR)
        {
            logger::error("Failed to hook firmware function!");
            Sleep(2000);
            exit(0); // NOLINT(concurrency-mt-unsafe)
        }
    }

    void game(DWORD eft_pid)
    {
        if (!should_spoof) return;
        logger::info("Patching HWID Check");
        HANDLE eft = OpenProcess(
            PROCESS_ALL_ACCESS, false, eft_pid);
        if (eft != nullptr)
        {
            // find GameAssembly.dll
            HMODULE found_module = nullptr;
            size_t found_module_size = 0;
            while (!found_module)
            {
                HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, eft_pid);
                if (snapshot != INVALID_HANDLE_VALUE)
                {
                    MODULEENTRY32W me32;
                    me32.dwSize = sizeof(MODULEENTRY32W);

                    if (Module32FirstW(snapshot, &me32))
                    {
                        do
                        {
                            if (std::wstring(me32.szModule) == std::wstring(GAMEASSEMBLY.begin(), GAMEASSEMBLY.end()))
                            {
                                found_module = reinterpret_cast<HMODULE>(me32.modBaseAddr);
                                found_module_size = me32.modBaseSize;
                                break;
                            }
                        }
                        while (Module32NextW(snapshot, &me32));
                    }
                    CloseHandle(snapshot);
                    Sleep(100);
                }
            }
            logger::info("GameAssembly Loaded");

            auto dumped_gameassembly = static_cast<char*>(malloc(found_module_size));
            if (!ReadProcessMemory(eft, found_module, dumped_gameassembly, found_module_size, nullptr))
            {
                free(dumped_gameassembly);
                CloseHandle(found_module);
                TerminateProcess(eft, 0);
                CloseHandle(eft);
                logger::error("Failed to dump GameAssembly");
                Sleep(2000);
                exit(0); // NOLINT(concurrency-mt-unsafe)
            }

            UINT64 http_function_offset = globals::find_pattern(NEEDS_HWID_CHECK_SIG, dumped_gameassembly,
                                                                found_module_size);
            if (http_function_offset)
            {
                logger::info("HWID Function Found");
                auto address = reinterpret_cast<PVOID>(reinterpret_cast<UINT64>(found_module) + // NOLINT(performance-no-int-to-ptr)
                    http_function_offset);

                // replace check function call by setting rax to 0 so we never check hwid
                byte shellcode[HWID_FUNCTION_PATCH_SIZE] = {
                    0x48, 0x31, 0xC0, // xor rax, rax
                    0x90, // nop
                    0x90 // nop 
                };

                if (!WriteProcessMemory(eft, address, shellcode, HWID_FUNCTION_PATCH_SIZE, nullptr))
                {
                    logger::error("HWID Function Patch Failed");
                    TerminateProcess(eft, 0);
                    CloseHandle(eft);
                    Sleep(2000);
                    exit(0); // NOLINT(concurrency-mt-unsafe)
                }
            }
            else
            {
                logger::error("HWID Function Not Found");
                TerminateProcess(eft, 0);
                CloseHandle(eft);
                Sleep(2000);
                exit(0); // NOLINT(concurrency-mt-unsafe)
            }
            free(dumped_gameassembly);
            CloseHandle(found_module);

            CloseHandle(eft);
            logger::info("HWID Resolution Patched");
        }
    }
}
