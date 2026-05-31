#pragma once
#include <random>
#include <string>
#include <sstream>

namespace globals
{
    const std::string DLL_CONFIG_FILE = "launcher.cfg";

    static int int_rand(const int& min, const int& max)
    {
        static std::mt19937* generator = nullptr;
        if (!generator) generator = new std::mt19937(clock());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(*generator);
    }

    static UINT64 find_pattern(const std::string& pattern, const char* buffer, size_t buffer_size)
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

        for (size_t i = 0; i < buffer_size - pattern_bytes; ++i)
        {
            bool match = true;
            for (size_t j = 0; j < pattern_bytes; ++j)
            {
                if (compiled_pattern[j] == 0) continue;
                if (compiled_pattern[j] != buffer[i + j])
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
        return 0;
    }
};
