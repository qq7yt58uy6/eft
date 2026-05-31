#include "dxhook.h"

#include <cstdint>
#include <ios>
#include <iostream>

#include "../menu/menu.h"
#include <Windows.h>

#include "../../external/detours/detours.h"
#include "../../util/logger.h"

// method based on kiero hook

namespace
{
    inline bool initDone = false;
    inline LRESULT(*oWndProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    inline void(*oExecuteCommandLists)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
    inline HRESULT(*oPresent11)(void* pSwapChain, UINT SyncInterval, UINT Flags);
    inline HRESULT(*oPresent12)(void* pSwapChain, UINT SyncInterval, UINT Flags);

    // Win10 + Win11 兼容索引
    int PRESENT_INDEX_11 = 8;
    int PRESENT_INDEX_12 = 146;
    int LIST_INDEX = 54;

    ID3D11Device* p_device11 = nullptr;
    ID3D11DeviceContext* p_context11 = nullptr;
    ID3D11RenderTargetView* main_render_target_view11 = nullptr;

    ID3D12Device* p_device12 = nullptr;
    ID3D12DescriptorHeap* p_descriptor_heap = nullptr;
    ID3D12DescriptorHeap* g_pd3d_rtv_desc_heap = nullptr;
    ID3D12DescriptorHeap* g_pd3d_srv_desc_heap = nullptr;
    ID3D12GraphicsCommandList* p_command_list = nullptr;
    ID3D12Resource** back_buffers = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE* descriptor_handles = nullptr;
    ID3D12CommandAllocator* p_allocator = nullptr;
    ID3D12CommandQueue* p_command_queue = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
    UINT bufferCount = 0;

    bool is_dx12 = false;
    UINT64* g_methods_table = nullptr;
    HWND imgui_window = nullptr;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI dxhook::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    if (!menu::is_active())
    {
        return oWndProc(hWnd, msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND dxhook::create_window(const HMODULE this_module)
{
    WNDCLASSEX window_class;
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = DefWindowProc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = this_module;
    window_class.hIcon = nullptr;
    window_class.hCursor = nullptr;
    window_class.hbrBackground = nullptr;
    window_class.lpszMenuName = nullptr;
    window_class.lpszClassName = L"DXWindow";
    window_class.hIconSm = nullptr;
    RegisterClassEx(&window_class);
    const HWND window = CreateWindowA("DXWindow",
        "DXWindow",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        0, 0,
        100, 100,
        nullptr,
        nullptr,
        this_module,
        nullptr);

    UnregisterClass(L"DXWindow", this_module);
    return window;
}

bool dxhook::init(const HMODULE this_module)
{
    return dx12_setup(this_module) || dx11_setup(this_module);
}

bool dxhook::dx12_setup(const HMODULE this_module)
{
    HMODULE lib_dxgi = GetModuleHandleW(L"dxgi.dll");
    HMODULE lib_d_3d12 = GetModuleHandleW(L"d3d12.dll");
    if (!lib_dxgi || !lib_d_3d12) return false;

    auto CreateDXGIFactory = reinterpret_cast<HRESULT(WINAPI*)(REFIID, void**)>(
        GetProcAddress(lib_dxgi, "CreateDXGIFactory1"));
    if (!CreateDXGIFactory) return false;

    IDXGIFactory4* factory = nullptr;
    if (CreateDXGIFactory(IID_PPV_ARGS(&factory)) < 0)
    {
        logger::error("FACTORY FAIL");
        return false;
    }

    IDXGIAdapter1* adapter = nullptr;
    if (factory->EnumAdapters1(0, &adapter) == DXGI_ERROR_NOT_FOUND)
    {
        logger::error("ADAPTER FAIL");
        return false;
    }

    auto D3D12CreateDevice = reinterpret_cast<HRESULT(WINAPI*)(IUnknown*, D3D_FEATURE_LEVEL, REFIID, void**)>(
        GetProcAddress(lib_d_3d12, "D3D12CreateDevice"));
    if (!D3D12CreateDevice) return false;

    ID3D12Device* device = nullptr;
    if (D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) < 0)
    {
        logger::error("DEVICE FAIL");
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    ID3D12CommandQueue* command_queue = nullptr;
    if (device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)) < 0)
    {
        logger::error("COMMAND QUEUE FAIL");
        return false;
    }

    ID3D12CommandAllocator* command_allocator = nullptr;
    if (device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)) < 0)
    {
        logger::error("COMMAND ALLOCATOR FAIL");
        return false;
    }

    ID3D12GraphicsCommandList* command_list = nullptr;
    if (device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, nullptr,
        IID_PPV_ARGS(&command_list)) < 0)
    {
        logger::error("COMMAND LIST FAIL");
        return false;
    }
    command_list->Close();

    const HWND window = create_window(this_module);
    if (!window)
    {
        logger::error("WINDOW FAIL");
        return false;
    }

    // 修复：DXGI_SWAP_CHAIN_DESC1 无 Windowed 字段
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.Width = 100;
    swap_chain_desc.Height = 100;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.SampleDesc.Count = 1;

    // 修复：全屏结构体单独声明
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
    fullscreenDesc.Windowed = TRUE;

    IDXGISwapChain1* swap_chain = nullptr;
    if (factory->CreateSwapChainForHwnd(command_queue, window, &swap_chain_desc, &fullscreenDesc, nullptr, &swap_chain) < 0)
    {
        logger::error("SWAPCHAIN FAIL");
        DestroyWindow(window);
        return false;
    }

    g_methods_table = (UINT64*)calloc(200, sizeof(UINT64));
    memcpy(g_methods_table, *(UINT64**)device, 44 * sizeof(UINT64));
    memcpy(g_methods_table + 44, *(UINT64**)command_queue, 19 * sizeof(UINT64));
    memcpy(g_methods_table + 63, *(UINT64**)command_allocator, 9 * sizeof(UINT64));
    memcpy(g_methods_table + 72, *(UINT64**)command_list, 60 * sizeof(UINT64));
    memcpy(g_methods_table + 132, *(UINT64**)swap_chain, 18 * sizeof(UINT64));

    device->Release();
    command_queue->Release();
    command_allocator->Release();
    command_list->Release();
    swap_chain->Release();
    factory->Release();
    adapter->Release();
    DestroyWindow(window);

    auto present_addr = (PVOID*)g_methods_table[PRESENT_INDEX_12];
    auto exec_addr = (PVOID*)g_methods_table[LIST_INDEX];

    oPresent12 = (HRESULT(*)(void*, UINT, UINT))present_addr;
    oExecuteCommandLists = (void(*)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*))exec_addr;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)oPresent12, dxPresent12);
    DetourAttach(&(PVOID&)oExecuteCommandLists, executeCommandLists12);

    if (DetourTransactionCommit() != NO_ERROR)
    {
        logger::error("DETOUR FAIL");
        free(g_methods_table);
        return false;
    }

    is_dx12 = true;
    return true;
}

bool dxhook::dx11_setup(const HMODULE this_module)
{
    HMODULE lib_d_3d11;
    if ((lib_d_3d11 = ::GetModuleHandle(L"d3d11.dll")) == nullptr)
    {
        return false;
    }

    void* d_3d11_create_device = reinterpret_cast<void*>(GetProcAddress(
        lib_d_3d11, "D3D11CreateDevice"));
    if (!d_3d11_create_device) return false;

    constexpr D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
    };

    ID3D11Device* device11 = nullptr;
    ID3D11DeviceContext* context11 = nullptr;
    D3D_FEATURE_LEVEL feature_level;

    HRESULT device_res = reinterpret_cast<HRESULT(*)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
        CONST D3D_FEATURE_LEVEL*, UINT, UINT, ID3D11Device**,
        D3D_FEATURE_LEVEL*, ID3D11DeviceContext**)>(d_3d11_create_device)(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            feature_levels,
            2,
            D3D11_SDK_VERSION,
            &device11,
            &feature_level,
            &context11);

    if (device_res < 0)
    {
        logger::error("DEVICE FAIL");
        return false;
    }

    IDXGIDevice* dxgi_device;
    if (device11->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device) < 0)
    {
        logger::error("DXGI DEVICE FAIL");
        return false;
    }

    IDXGIAdapter* dxgi_adapter;
    if (dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter) < 0)
    {
        logger::error("ADAPTER FAIL");
        return false;
    }

    IDXGIFactory2* factory;
    if (dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&factory) < 0)
    {
        logger::error("FACTORY FAIL");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1 = {};
    swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swap_chain_desc1.BufferCount = 2;
    swap_chain_desc1.Width = 100;
    swap_chain_desc1.Height = 100;
    swap_chain_desc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc1.SampleDesc.Count = 1;

    const HWND window = create_window(this_module);
    if (!window)
    {
        logger::error("WINDOW FAIL");
        return false;
    }

    IDXGISwapChain1* swap_chain11;
    if (factory->CreateSwapChainForHwnd(device11, window, &swap_chain_desc1, nullptr, nullptr, &swap_chain11) < 0)
    {
        logger::error("SWAPCHAIN FAIL");
        DestroyWindow(window);
        return false;
    }

    g_methods_table = static_cast<UINT64*>(calloc(205, sizeof(UINT64)));
    memcpy(g_methods_table, *reinterpret_cast<UINT64**>(swap_chain11), 18 * sizeof(UINT64));
    memcpy(g_methods_table + 18, *reinterpret_cast<UINT64**>(device11), 43 * sizeof(UINT64));
    memcpy(g_methods_table + 18 + 43, *reinterpret_cast<UINT64**>(context11), 144 * sizeof(UINT64));

    swap_chain11->Release();
    device11->Release();
    context11->Release();
    DestroyWindow(window);

    const auto target = reinterpret_cast<PVOID*>(g_methods_table[PRESENT_INDEX_11]);
    oPresent11 = reinterpret_cast<HRESULT(*)(void*, UINT, UINT)>(target);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)oPresent11, dxPresent11);

    if (DetourTransactionCommit() != NO_ERROR)
    {
        logger::error("DETOUR FAIL");
        free(g_methods_table);
        return false;
    }

    return true;
}

void dxhook::shutdown()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourDetach(&(PVOID&)oPresent11, dxPresent11);
    DetourDetach(&(PVOID&)oPresent12, dxPresent12);
    DetourDetach(&(PVOID&)oExecuteCommandLists, executeCommandLists12);

    DetourTransactionCommit();
    free(g_methods_table);
    g_methods_table = nullptr;
}

void __fastcall dxhook::executeCommandLists12(ID3D12CommandQueue* p_command_queue1, UINT num_command_lists,
    ID3D12CommandList* const* pp_command_lists)
{
    if (!p_command_queue)
    {
        p_command_queue = p_command_queue1;
    }
    oExecuteCommandLists(p_command_queue1, num_command_lists, pp_command_lists);
}

HRESULT __fastcall dxhook::dxPresent11(IDXGISwapChain* p_swap_chain, UINT sync_interval, UINT flags)
{
    if (!initDone)
    {
        if (SUCCEEDED(p_swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&p_device11))))
        {
            is_dx12 = false;
            p_device11->GetImmediateContext(&p_context11);
            DXGI_SWAP_CHAIN_DESC sd;
            p_swap_chain->GetDesc(&sd);
            imgui_window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer;
            p_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
            p_device11->CreateRenderTargetView(pBackBuffer, nullptr, &main_render_target_view11);
            pBackBuffer->Release();

            oWndProc = (LRESULT(*)(HWND, UINT, WPARAM, LPARAM))SetWindowLongPtr(
                imgui_window, GWLP_WNDPROC, (LONG_PTR)WndProc);

            init_imgui();
            initDone = true;
        }
    }

    if (!is_dx12)
        draw_imgui(nullptr);

    return oPresent11(p_swap_chain, sync_interval, flags);
}

HRESULT __fastcall dxhook::dxPresent12(IDXGISwapChain3* p_swap_chain, UINT sync_interval, UINT flags)
{
    if (!initDone)
    {
        if (SUCCEEDED(p_swap_chain->GetDevice(__uuidof(ID3D12Device), reinterpret_cast<void**>(&p_device12))))
        {
            is_dx12 = true;
            DXGI_SWAP_CHAIN_DESC sd;
            p_swap_chain->GetDesc(&sd);
            imgui_window = sd.OutputWindow;

            DXGI_SWAP_CHAIN_DESC swapChainDesc;
            p_swap_chain->GetDesc(&swapChainDesc);
            bufferCount = swapChainDesc.BufferCount;

            p_device12->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&p_allocator));
            p_device12->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, p_allocator, nullptr,
                IID_PPV_ARGS(&p_command_list));
            p_command_list->Close();

            D3D12_DESCRIPTOR_HEAP_DESC desc;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = bufferCount;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 0;
            p_device12->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3d_rtv_desc_heap));

            SIZE_T rtvDescriptorSize = p_device12->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            rtv_handle = g_pd3d_rtv_desc_heap->GetCPUDescriptorHandleForHeapStart();

            back_buffers = new ID3D12Resource * [bufferCount];
            descriptor_handles = new D3D12_CPU_DESCRIPTOR_HANDLE[bufferCount];

            for (UINT i = 0; i < bufferCount; i++)
            {
                ID3D12Resource* pBackBuffer = nullptr;
                p_swap_chain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
                p_device12->CreateRenderTargetView(pBackBuffer, nullptr, rtv_handle);
                back_buffers[i] = pBackBuffer;
                descriptor_handles[i] = rtv_handle;
                rtv_handle.ptr += rtvDescriptorSize;
            }

            oWndProc = (LRESULT(*)(HWND, UINT, WPARAM, LPARAM))SetWindowLongPtr(
                imgui_window, GWLP_WNDPROC, (LONG_PTR)WndProc);

            init_imgui();
            initDone = true;
        }
    }

    if (is_dx12)
        draw_imgui(p_swap_chain);

    return oPresent12(p_swap_chain, sync_interval, flags);
}

void dxhook::init_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplWin32_Init(imgui_window);

    if (is_dx12)
    {
        D3D12_DESCRIPTOR_HEAP_DESC descriptor = {};
        descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descriptor.NumDescriptors = 1;
        descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        p_device12->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(&p_descriptor_heap));
        ImGui_ImplDX12_Init(p_device12, bufferCount, DXGI_FORMAT_R8G8B8A8_UNORM, p_descriptor_heap,
            p_descriptor_heap->GetCPUDescriptorHandleForHeapStart(),
            p_descriptor_heap->GetGPUDescriptorHandleForHeapStart());
    }
    else
    {
        ImGui_ImplDX11_Init(p_device11, p_context11);
    }

    menu::setup();
    initDone = true;
}

void dxhook::shutdown_imgui()
{
    initDone = false;

    if (is_dx12)
    {
        ImGui_ImplDX12_Shutdown();
    }
    else
    {
        ImGui_ImplDX11_Shutdown();
    }

    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (oWndProc != nullptr && imgui_window != nullptr)
    {
        SetWindowLongPtr(imgui_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
    }

    if (is_dx12)
    {
        if (p_descriptor_heap) p_descriptor_heap->Release();
        if (p_allocator) p_allocator->Release();
        if (p_command_list) p_command_list->Release();

        for (size_t i = 0; i < bufferCount; ++i)
        {
            if (back_buffers[i]) back_buffers[i]->Release();
        }

        delete[] back_buffers;
        delete[] descriptor_handles;
    }
    else
    {
        if (main_render_target_view11) main_render_target_view11->Release();
    }
}

void dxhook::draw_imgui(IDXGISwapChain3* p_swap_chain)
{
    if (is_dx12)
    {
        ImGui_ImplDX12_NewFrame();
    }
    else
    {
        ImGui_ImplDX11_NewFrame();
    }

    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    menu::draw();
    ImGui::EndFrame();
    ImGui::Render();

    if (is_dx12)
    {
        size_t index = p_swap_chain->GetCurrentBackBufferIndex();
        ID3D12Resource* p_back_buffer = back_buffers[index];

        p_allocator->Reset();
        p_command_list->Reset(p_allocator, nullptr);

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = p_back_buffer;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        p_command_list->ResourceBarrier(1, &barrier);

        p_command_list->OMSetRenderTargets(1, &descriptor_handles[index], FALSE, nullptr);
        p_command_list->SetDescriptorHeaps(1, &p_descriptor_heap);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), p_command_list);

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        p_command_list->ResourceBarrier(1, &barrier);
        p_command_list->Close();

        if (p_command_queue)
        {
            p_command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&p_command_list);
        }
    }
    else
    {
        p_context11->OMSetRenderTargets(1, &main_render_target_view11, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}