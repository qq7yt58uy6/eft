#pragma once
#include <imgui.h>
#include <sstream>
#include <vector>

#include "ConfigValue.h"
#include "../gui/menu/imgui_addons.h"
#include "../util/utils.h"

#pragma pack(push, 1)
class ColorpickerValue : public ConfigValue
{
public:
    ColorpickerValue(const ImVec4 initial, const std::string& n) : ConfigValue(n)
    {
        this->value_ = initial;
        this->def_ = initial;
    }

    void draw() override
    {
        imgui_addons::colorpicker(this->get_imgui_title(), &this->value_);
    }

    void set(const std::string& string) override
    {
        this->value_ = from_string(string);
    }

    std::string get() override
    {
        return to_string(this->value_);
    }

    std::string get_default() override
    {
        return to_string(this->def_);
    }

    [[nodiscard]] ImVec4 get_value() const
    {
        return this->value_;
    }

private:
    ImVec4 value_;
    ImVec4 def_;

    static std::string to_string(ImVec4 v)
    {
        std::stringstream ss;
        ss << v.x << "|" << v.y << "|" << v.z << "|" << v.w;
        return ss.str();
    }

    static ImVec4 from_string(const std::string& string)
    {
        std::vector<std::string> elements = utils::split(string, '|');
        return {std::stof(elements[0]), std::stof(elements[1]), std::stof(elements[2]), std::stof(elements[3])};
    }
};
#pragma pack(pop)
