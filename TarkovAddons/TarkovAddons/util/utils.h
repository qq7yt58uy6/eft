#pragma once
#include <Windows.h>

namespace utils
{
    static void get_desktop_resolution(int& horizontal, int& vertical)
    {
        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);
        horizontal = desktop.right;
        vertical = desktop.bottom;
    }

    static void send_input_vk(unsigned short key)
    {
        INPUT input{
            .type = INPUT_KEYBOARD
        }; // NOLINT(clang-diagnostic-missing-designated-field-initializers)
        input.ki = KEYBDINPUT{
            .wVk = key,
            .wScan = 0,
            .dwFlags = 0,
            .dwExtraInfo = 0,
        }; // NOLINT(clang-diagnostic-missing-designated-field-initializers)
        SendInput(1, &input, sizeof(INPUT));
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }

    static bool is_key_down(int key)
    {
        return (GetAsyncKeyState(key) & 1) != 0;
    }

    static bool is_key_held(int key)
    {
        return ((GetAsyncKeyState(key) >> 15) & 0x0001) == 0x0001;
    }

    static std::vector<std::string> split(const std::string& s, char delim)
    {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;

        while (getline(ss, item, delim))
        {
            result.push_back(item);
        }

        return result;
    }
}
