#pragma once
#include "ConfigValue.h"
#include "../gui/menu/imgui_addons.h"

#pragma pack(push, 1)
class TextboxValue : public ConfigValue
{
public:
    TextboxValue(const std::string& initial, const std::string& n) : ConfigValue(n)
    {
        this->value_ = initial;
        this->def_ = initial;
    }

    void draw() override
    {
        imgui_addons::textbox(this->get_imgui_title(), &this->value_);
    }

    void set(const std::string& string) override
    {
        this->value_ = string;
    }

    std::string get() override
    {
        return this->value_;
    }

    std::string get_default() override
    {
        return this->def_;
    }

private:
    std::string value_;
    std::string def_;
};
#pragma pack(pop)
