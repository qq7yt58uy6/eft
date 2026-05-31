#pragma once
#include <functional>

#include "CheckboxValue.h"
#include "ConfigValue.h"

#pragma pack(push, 1)
class ActionRowValue : public ConfigValue
{
public:
    ActionRowValue(const std::function<void ()>& action, const std::string& n,
                   const bool sameline = false) : ConfigValue(n)
    {
        this->action_ = action;
        this->sameline_ = sameline;
    }

    void draw() override
    {
        if (this->sameline_) ImGui::SameLine();
        if (ImGui::Button(this->get_imgui_title().c_str()))
        {
            this->action_();
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y * 3 * menu::get_scale_factor());
    }

    void set(const std::string& string) override
    {
    }

    std::string get() override
    {
        return "";
    }

    std::string get_default() override
    {
        return "";
    }

private:
    std::function<void ()> action_;
    bool sameline_;
};
#pragma pack(pop)
