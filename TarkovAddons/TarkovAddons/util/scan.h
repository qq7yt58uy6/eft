#pragma once

#include <string>
#include <windows.h>

#include "globals.h"
#include "logger.h"

namespace scan
{
    static UINT64 find_pattern_internal(const std::string& pattern, const char* module_start, size_t module_size)
    {
        if (pattern.length() < 2) return 0;

        size_t pattern_bytes = std::count(pattern.begin(), pattern.end(), ' ') + 1;
        auto compiled_pattern = new char[pattern_bytes];
        size_t string_index = 0;
        size_t byte_index = 0;
        do
        {
            if (pattern[string_index] == '?')
            {
                string_index += 2;
                compiled_pattern[byte_index] = 0;
                byte_index += 1;
                continue;
            }
            std::string byte_string = pattern.substr(string_index, 2);
            string_index += 3;
            char byte_value = static_cast<char>(strtoul(byte_string.c_str(),
                                                        reinterpret_cast<char**>(reinterpret_cast<UINT64>(byte_string. // NOLINT(performance-no-int-to-ptr)
                                                                c_str()) +
                                                            byte_string.length()), 16));
            compiled_pattern[byte_index] = byte_value;
            byte_index += 1;
        }
        while (string_index < pattern.length());

        for (size_t i = 0; i < module_size - pattern_bytes; ++i)
        {
            bool match = true;
            for (size_t j = 0; j < pattern_bytes; ++j)
            {
                if (compiled_pattern[j] == 0) continue;
                if (compiled_pattern[j] != module_start[i + j])
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                delete[] compiled_pattern;
                return i;
            }
        }

        delete[] compiled_pattern;
        logger::error("Pattern not found: " + pattern);
        return 0;
    }

    static UINT64 find_pattern(const std::string& pattern, const char* module_start, size_t module_size)
    {
        UINT64 res = find_pattern_internal(pattern, module_start, module_size);
        if (res)
        {
            if (globals::verbose) logger::info("Found Pattern: " + pattern);
        }
        return reinterpret_cast<UINT64>(module_start) + res;
    }

    static UINT64 find_pattern_xref(const std::string& pattern, const char* module_start, size_t module_size,
                                    size_t offset, size_t instruction_size)
    {
        const INT64 res = static_cast<INT64>(find_pattern_internal(pattern, module_start, module_size));
        if (!res) return 0;
        const INT64 location = reinterpret_cast<INT64>(module_start) + res + offset; // NOLINT(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
        if (location < reinterpret_cast<INT64>(module_start) || location > static_cast<INT64>(reinterpret_cast<UINT64>(
            module_start) + module_size - sizeof(INT32)))
        {
            logger::error("Xref out of bounds: " + pattern);
            return 0;
        }
        const INT64 relative = *reinterpret_cast<int32_t*>(location); // NOLINT(performance-no-int-to-ptr)
        if (globals::verbose) logger::info("Found Xref: " + pattern);
        return reinterpret_cast<UINT64>(module_start) + res + relative + instruction_size; //  - 0x0000000100000000
    }

    inline UINT64 import(std::string name)
    {
        UINT64 res = reinterpret_cast<UINT64>(GetProcAddress(GetModuleHandleA("GameAssembly.dll"), name.c_str()));
        if (!res)
        {
            logger::error("Import Failed: " + name);
        }
        else
        {
            if (globals::verbose) logger::info("Imported: " + name);
        }
        return res;
    }
}
