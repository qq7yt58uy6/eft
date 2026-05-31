#pragma once
#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <functional>
#include <vector>
#include <cfloat>  // 用于FLT_MAX

#include "menu.h"

namespace imgui_addons
{
    // 修复：自定义字体+字号 居中文本（中文不偏移）
    static void centered_text(ImDrawList* draw_list, ImFont* font, float font_size, const ImVec2& pos, const ImVec4 col,
        const std::string& text)
    {
        // 用当前字体+字号计算尺寸，适配中文
        const ImVec2 size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text.c_str());
        draw_list->AddText(font, font_size, { pos.x - size.x / 2, pos.y - size.y / 2 }, ImGui::GetColorU32(col),
            text.c_str());
    }

    // 修复：默认字体 居中文本
    static void centered_text(ImDrawList* draw_list, const ImVec2& pos, const ImVec4 col, const std::string& text)
    {
        const ImVec2 size = ImGui::CalcTextSize(text.c_str());
        draw_list->AddText({ pos.x - size.x / 2, pos.y - size.y / 2 }, ImGui::GetColorU32(col), text.c_str());
    }

    // 修复：标签页支持中文
    static void tab(const std::string& label, const bool selected, const std::function<void()>& action)
    {
        ImGui::PushFont(menu::get_font());
        if (selected) ImGui::PushStyleColor(ImGuiCol_Tab, ImGui::GetStyle().Colors[ImGuiCol_TabActive]);
        if (ImGui::TabItemButton(label.c_str()))
        {
            action();
        }
        if (selected) ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    // 修复：下拉框支持中文
    static bool select(const std::string& label, int* v, const std::vector<const char*>& options)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushFont(menu::get_font());  // 中文字体

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        std::string just_text = label.substr(0, label.find("##"));
        ImGui::Text(just_text.c_str());

        float width = 200 * menu::get_scale_factor();
        ImGui::SameLine(ImGui::GetWindowWidth() - width - 4 * menu::get_scale_factor());
        ImGui::SetNextItemWidth(width);
        auto res = ImGui::Combo(std::string("##" + label).c_str(), v, options.data(),
            static_cast<int32_t>(options.size()));

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        ImGui::Dummy({ 0, 0 });

        ImGui::PopFont();
        ImGui::PopID();
        return res;
    }

    // 修复：浮点滑块支持中文
    static bool slider_float(const std::string& label, float* v, float min, float max)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushFont(menu::get_font());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        std::string just_text = label.substr(0, label.find("##"));
        ImGui::Text(just_text.c_str());

        float width = 200 * menu::get_scale_factor();
        ImGui::SameLine(ImGui::GetWindowWidth() - width - 4 * menu::get_scale_factor());
        ImGui::SetNextItemWidth(width);
        auto res = ImGui::SliderFloat(std::string("##" + label).c_str(), v, min, max);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        ImGui::Dummy({ 0, 0 });

        ImGui::PopFont();
        ImGui::PopID();
        return res;
    }

    // 修复：整数滑块支持中文
    static bool slider_int(const std::string& label, int* v, int min, int max)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushFont(menu::get_font());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        std::string just_text = label.substr(0, label.find("##"));
        ImGui::Text(just_text.c_str());

        float width = 200 * menu::get_scale_factor();
        ImGui::SameLine(ImGui::GetWindowWidth() - width - 4 * menu::get_scale_factor());
        ImGui::SetNextItemWidth(width);
        auto res = ImGui::SliderInt(std::string("##" + label).c_str(), v, min, max);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        ImGui::Dummy({ 0, 0 });

        ImGui::PopFont();
        ImGui::PopID();
        return res;
    }

    // 修复：颜色选择器支持中文
    static bool colorpicker(const std::string& label, ImVec4* v)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushFont(menu::get_font());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        std::string just_text = label.substr(0, label.find("##"));
        ImGui::Text(just_text.c_str());

        float width = 200 * menu::get_scale_factor();
        ImGui::SameLine(ImGui::GetWindowWidth() - width - 4 * menu::get_scale_factor());
        ImGui::SetNextItemWidth(width);
        auto res = ImGui::ColorEdit4(std::string("##" + label).c_str(), reinterpret_cast<float*>(v),
            ImGuiColorEditFlags_PickerHueWheel);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        ImGui::Dummy({ 0, 0 });

        ImGui::PopFont();
        ImGui::PopID();
        return res;
    }

    // 修复：整数输入框支持中文 + 修正拼写错误
    static bool int_textbox(const std::string& label, int* v)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushFont(menu::get_font());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        std::string just_text = label.substr(0, label.find("##"));
        ImGui::Text(just_text.c_str());

        float width = 200 * menu::get_scale_factor();
        ImGui::SameLine(ImGui::GetWindowWidth() - width - 4 * menu::get_scale_factor());
        ImGui::SetNextItemWidth(width);
        auto res = ImGui::InputInt(std::string("##" + label).c_str(), v);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        ImGui::Dummy({ 0, 0 });

        ImGui::PopFont();
        ImGui::PopID();
        return res;
    }

    // 修复：文本输入框支持中文
    static bool textbox(const std::string& label, std::string* v)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushFont(menu::get_font());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        std::string just_text = label.substr(0, label.find("##"));
        ImGui::Text(just_text.c_str());

        float width = 200 * menu::get_scale_factor();
        ImGui::SameLine(ImGui::GetWindowWidth() - width - 4 * menu::get_scale_factor());
        ImGui::SetNextItemWidth(width);
        auto res = ImGui::InputText(std::string("##" + label).c_str(), v);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        ImGui::Dummy({ 0, 0 });

        ImGui::PopFont();
        ImGui::PopID();
        return res;
    }

    // 核心修复：复选框支持中文（解决CheckboxValue不显示中文）
    static bool checkbox(const std::string& label, bool* v)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushFont(menu::get_font());  // 绑定中文字体
        float font_size = 16.f * menu::get_scale_factor();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        std::string just_text = label.substr(0, label.find("##"));
        ImGui::Text(just_text.c_str());  // 中文标签正常显示

        std::string state_text = *v ? "开启" : "关闭";
        ImVec2 text_rect = ImGui::CalcTextSize(state_text.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - text_rect.x - 4 * menu::get_scale_factor());
        ImVec2 p = ImGui::GetCursorScreenPos();

        auto color_hovered = ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TabActive]);
        auto color_inactive = ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TabHovered]);
        auto color_active_hovered_t = ImGui::GetStyle().Colors[ImGuiCol_CheckMark];
        color_active_hovered_t.x += 0.15f;
        color_active_hovered_t.y += 0.15f;
        color_active_hovered_t.z += 0.15f;
        auto color_active_hovered = ImGui::GetColorU32(color_active_hovered_t);
        auto color_active = ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_CheckMark]);

        ImGui::InvisibleButton(label.c_str(), text_rect);
        bool hovered = ImGui::IsItemHovered();
        bool clicked = ImGui::IsItemClicked();
        if (clicked)
            *v = !(*v);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        // 修复：使用中文字体绘制文本
        draw_list->AddText(
            menu::get_font(), font_size,
            p, !*v ? (hovered ? color_active_hovered : color_active) : (hovered ? color_hovered : color_inactive),
            state_text.c_str()
        );

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 2 * menu::get_scale_factor());
        ImGui::Dummy({ 0, 0 });

        ImGui::PopFont();
        ImGui::PopID();
        return clicked;
    }
};