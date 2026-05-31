// 引入标准输入输出库
#include <iostream>
// 引入Windows系统API
#include <windows.h>

// 资源包相关
#include "assets/asset_bundles.h"
// DX渲染钩子（界面绘制）
#include "gui/dxhook/dxhook.h"
// 游戏钩子管理
#include "hooks/hooks.h"
// Unity游戏IL2CPP框架相关
#include "il2cpp/unity.h"
// 常驻功能模块
#include "module/always_on.h"
// 通用钩子
#include "module/common_hooks.h"
// 模块管理器
#include "module/ModuleManager.h"
// 全局变量/配置
#include "util/globals.h"
// 日志工具
#include "util/logger.h"

// 匿名命名空间（内部函数）
namespace
{
    
    // 控制台文件句柄
    FILE* console = nullptr;

    // 显示控制台窗口
    void show_console()
    {
        AllocConsole(); // 创建控制台
        SetConsoleOutputCP(CP_UTF8);
        errno_t _ = freopen_s(&console, "CONOUT$", "w", stdout); // 重定向输出
        // 设置控制台标题
        SetConsoleTitleA(std::string(globals::get_display_title() + " | " + globals::credits).c_str());
    }

    // 隐藏控制台窗口
    void hide_console()
    {
        errno_t _ = fclose(console);
        FreeConsole(); // 释放控制台
    }

    // 主初始化函数（不返回）
    [[noreturn]] void init(HMODULE this_module)
    {
        Sleep(5000); // 等待5秒，让游戏完全启动

        // 如果开启控制台，则显示
        if (globals::console) show_console();

        // 打印标题与版权信息
        logger::info(globals::get_display_title());
        logger::info(globals::credits);

        // 等待游戏核心DLL加载
        logger::info("等待 GameAssembly.dll 加载");
        while (!GetModuleHandleA("GameAssembly.dll"))
        {
            Sleep(50);
        }

        // 初始化IL2CPP偏移（游戏数据）
        il2cpp_offsets::init();

        // 等待DX渲染组件加载
        while (!GetModuleHandle(L"d3d11.dll") || !GetModuleHandle(L"dxgi.dll"))
        {
            Sleep(50);
        }

        // 初始化Unity交互
        logger::info("正在加载 Unity 模块");
        unity::init();
        logger::info("Unity 加载完成");

        // 安装DX钩子，用于绘制菜单界面
        logger::info("正在安装 Present 钩子");
        if (!dxhook::init(this_module))
            logger::error("Present 钩子安装失败");
        else
            logger::info("Present 钩子安装成功");

        // 加载所有功能模块
        logger::info("正在加载功能模块");
        ModuleManager::get_instance()->init();
        logger::info("功能模块加载完成");

        // 加载默认功能与通用钩子
        logger::info("正在加载默认功能");
        always_on::init();
        common_hooks::init();
        logger::info("默认功能加载完成");

        //std::cout << "\n";
        // 防诈骗提示
        //logger::warn("这是免费软件！如果你为此付费或完成任务才能获取，你被骗了！");
        std::cout << "\n";
        // 提示菜单快捷键
        logger::info("菜单快捷键: Insert");

        std::cout << "\n";

        logger::info("卸载快捷键: Del");

        std::cout << "\n";
        // 退出线程（卸载功能将在后续添加）
        //ExitThread(0);

        // 等待卸载快捷键
        while (true)
        {
            Sleep(50);
            // 按下 Delete 键卸载
            if (GetAsyncKeyState(VK_DELETE) & 1)
            {
                logger::info("正在卸载...");
                break;
            }
        }

        // 关闭钩子
        logger::info("已卸载 Present 钩子");
        dxhook::shutdown();

        // 关闭ImGui界面
        logger::info("正在关闭 ImGui");
        dxhook::shutdown_imgui();

        // 卸载所有钩子
        logger::info("已卸载所有钩子");
        hooks::unhook_all();

        // 销毁模块管理器
        delete ModuleManager::get_instance();

        // 隐藏控制台
        if (globals::console) hide_console();

        // 完全卸载DLL
        FreeLibraryAndExitThread(this_module, 0);
    }
}

// DLL入口函数（Windows标准）
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        // DLL被注入时执行
    case DLL_PROCESS_ATTACH:
        // 创建线程，运行初始化
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(init), hModule, 0, nullptr);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    default:;
    }

    return TRUE;
}