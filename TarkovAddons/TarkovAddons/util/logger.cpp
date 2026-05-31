#include "logger.h"

#include <iostream>
#include <windows.h>

namespace
{
    void set_color(int textColor, int bgColor)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole,
                                static_cast<WORD>(bgColor << 4) | textColor);
    }

    void reset_color() { set_color(7, 0); }
}

namespace logger
{
    void info(std::string msg)
    {
        std::cout << "[";
        set_color(3, 0);
        std::cout << "INFO";
        reset_color();
        std::cout << "] ";
        std::cout << msg << "\n";
    }

    void warn(std::string msg)
    {
        std::cout << "[";
        set_color(6, 0);
        std::cout << "WARN";
        reset_color();
        std::cout << "] ";
        std::cout << msg << "\n";
    }

    void error(std::string msg)
    {
        std::cout << "[";
        set_color(4, 0);
        std::cout << "ERROR";
        reset_color();
        std::cout << "] ";
        std::cout << msg << "\n";
    }
}
