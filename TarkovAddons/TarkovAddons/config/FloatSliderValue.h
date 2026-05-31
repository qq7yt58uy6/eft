#pragma once
#include "ConfigValue.h"
#include "../gui/menu/imgui_addons.h"

#pragma pack(push, 1)
class FloatSliderValue : public ConfigValue
{
public:
    FloatSliderValue(float initial, float min, float max, const std::string& n) : ConfigValue(n)
    {
        this->value_ = initial;
        this->def_ = initial;
        this->min_ = min;
        this->max_ = max;
    }

    void draw() override
    {
        imgui_addons::slider_float(this->get_imgui_title(), &value_, this->min_, this->max_);
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

    [[nodiscard]] float get_value() const
    {
        return this->value_;
    }

private:
    float value_;
    float def_;
    float min_;
    float max_;

    static std::string to_string(float v)
    {
        return std::to_string(v);
    }

    static float from_string(const std::string& string)
    {
        return std::stof(string);
    }
};
#pragma pack(pop)
