#include "menu.h"

#include "resources.h"
#include "../../module/ModuleManager.h"
#include "../../util/globals.h"
#include "../../util/utils.h"

namespace menu
{
    namespace
    {
        bool draw_imgui = false;
        float widthh;
        float heightt;
        float scale_factor;
        float font_size;
        float large_font_size;
        float hud_font_size;
        ImFont* poppins_regular;
        ImFont* poppins_regular_hud;
        ImFont* sfui_light;
        ImFont* sfui_bold;
        ImFont* icons;

        ModuleCategory selected_tab = Aimbot;
    }

    // 菜单主字体（中文核心）
    ImFont* get_font()
    {
        return poppins_regular;
    }

    float get_hud_font_size()
    {
        return hud_font_size;
    }

    ImFont* get_hud_font()
    {
        return poppins_regular_hud;
    }

    float get_scale_factor()
    {
        return scale_factor;
    }

    float get_width()
    {
        return widthh;
    }

    float get_height()
    {
        return heightt;
    }

    bool is_active()
    {
        return draw_imgui == true;
    }

    void setup()
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        
        int horizontal = 0;
        int vertical = 0;
        utils::get_desktop_resolution(horizontal, vertical);
        widthh = static_cast<float>(horizontal);
        heightt = static_cast<float>(vertical);
        scale_factor = (static_cast<float>(horizontal) / 1920.0f + static_cast<float>(vertical) / 1080.0f) / 2.0f;
        font_size = 16 * scale_factor;
        large_font_size = 18 * scale_factor;
        hud_font_size = 256 * scale_factor;

        // 加载微软雅黑中文字体（完整中文支持）
        poppins_regular = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/msyh.ttc", font_size, NULL, io.Fonts->GetGlyphRangesChineseFull());
       
        poppins_regular_hud = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/msyh.ttc", hud_font_size, NULL, io.Fonts->GetGlyphRangesChineseFull());

        // 加载其他字体
        sfui_light = io.Fonts->AddFontFromMemoryCompressedTTF(light_compressed_data, light_compressed_size, font_size);
        sfui_bold = io.Fonts->AddFontFromMemoryCompressedTTF(bold_compressed_data, bold_compressed_size, large_font_size);
        icons = io.Fonts->AddFontFromMemoryCompressedTTF(icons_compressed_data, icons_compressed_size, font_size);

        // ✅【核心修复】必须构建字体纹理！不加中文永远不显示
        //io.Fonts->Build();
    }

    void draw()
    {
        //auto* dl = ImGui::GetBackgroundDrawList();
        //dl->AddText(poppins_regular, 32.0f, ImVec2(50, 50), IM_COL32(255, 0, 0, 255), "强制中文测试");
        //const char test_str[] = "\xE5\xBC\xBA\xE5\x88\xB6\xE4\xB8\xAD\xE6\x96\x87\xE6\xB5\x8B\xE8\xAF\x95";
        //dl->AddText(poppins_regular, 32.0f, ImVec2(50, 50), IM_COL32(255, 0, 0, 255), test_str);
        theme();
        ImGui::StyleColorsClassic();
        ImGui::GetMouseCursor();
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        ImGui::GetIO().WantCaptureMouse = is_active();

        if (is_active())
        {
            constexpr float width = 800;
            constexpr float height = 600;
            ImGui::SetNextWindowSize(ImVec2(width * scale_factor, height * scale_factor), ImGuiCond_Once);
            ImGui::SetNextWindowBgAlpha(1.0f);

            if (ImGui::Begin(globals::get_display_title().c_str(), &draw_imgui,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
            {
                // ✅【核心修复】全局推送中文字体（整个菜单生效）
                ImGui::PushFont(menu::get_font());

                // 标签页
                ImGui::BeginTabBar("");
                imgui_addons::tab("自瞄", selected_tab == Aimbot, []
                    {
                        selected_tab = Aimbot;
                    });
                imgui_addons::tab("透视", selected_tab == Visuals, []
                    {
                        selected_tab = Visuals;
                    });
                imgui_addons::tab("功能", selected_tab == Exploit, []
                    {
                        selected_tab = Exploit;
                    });
                imgui_addons::tab("杂项", selected_tab == Misc, []
                    {
                        selected_tab = Misc;
                    });
                imgui_addons::tab("配置", selected_tab == Config, []
                    {
                        selected_tab = Config;
                    });
                ImGui::EndTabBar();

                bool overflow = false;
                ImVec2 window_pos = ImGui::GetWindowPos();
                ImVec2 window_size = ImGui::GetWindowSize();
                float window_min = window_pos.x + ImGui::GetCursorPosX();
                float window_max = window_pos.x + window_size.x;
                float window_space = window_max - window_min;
                ImGui::BeginChild("##Child1", { 0.5f * window_space - 4 * scale_factor, 0 });

                ModuleManager::get_instance()->apply([window_space, height, &overflow](Module* m)
                    {
                        if (m->category != selected_tab) return;

                        ImGui::Dummy({ 0, 3 * scale_factor });
                        ImGui::Indent();

                        // ✅【修复】使用支持中文的菜单字体
                        ImGui::PushFont(menu::get_font());
                        ImGui::Text(m->name.c_str());
                        ImGui::Dummy({ 0, 4 * scale_factor });
                        ImGui::PopFont();

                        ImGui::Unindent();

                        // 绘制配置项（Checkbox/Slider 中文正常）
                        m->apply([](ConfigValue* c)
                            {
                                ImGui::Indent();
                                c->draw();
                                ImGui::Unindent();
                            });

                        ImGui::Dummy({ 0, 0 });

                        if (ImGui::GetCursorPosY() >= 0.5f * height * scale_factor)
                        {
                            overflow = true;
                            ImGui::EndChild();
                            ImGui::SameLine();
                            ImGui::BeginChild("##Child2", { 0.5f * window_space - 4 * scale_factor, 0 });
                        }
                    });

                if (!overflow)
                {
                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild("##Child2", { 0.5f * window_space - 4 * scale_factor, 0 });
                }
                ImGui::EndChild();

                // ✅【核心修复】弹出全局字体
                ImGui::PopFont();
            }
            ImGui::End();
        }

        //  Insert 开关菜单
        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            draw_imgui = !draw_imgui;
        }

        // 绘制HUD
        auto draw_list = ImGui::GetBackgroundDrawList();
        ModuleManager::get_instance()->apply([draw_list](Module* m)
            {
                m->draw_overlay(draw_list);
            });
    }
}