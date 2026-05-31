#pragma once
#include "ConfigValue.h"
#include "../gui/menu/imgui_addons.h"

#pragma pack(push, 1)
class IntSliderValue : public ConfigValue
{
public:
    IntSliderValue(int initial, int min, int max, const std::string& n) : ConfigValue(n)
    {
        this->value_ = initial;
        this->def_ = initial;
        this->min_ = min;
        this->max_ = max;
    }

    void draw() override
    {
        imgui_addons::slider_int(this->get_imgui_title(), &this->value_, this->min_, this->max_);
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

    [[nodiscard]] int get_value() const
    {
        return this->value_;
    }

private:
    int value_;
    int def_;
    int min_;
    int max_;

    static std::string to_string(int v)
    {
        return std::to_string(v);
    }

    static int from_string(const std::string& string)
    {
        return std::stoi(string);
    }
};
#pragma pack(pop)
