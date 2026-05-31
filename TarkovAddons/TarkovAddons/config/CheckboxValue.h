#pragma once
#include <sstream>

#include "ConfigValue.h"
#include "../gui/menu/imgui_addons.h"

#pragma pack(push, 1)
class CheckboxValue : public ConfigValue
{
public:
    CheckboxValue(const bool initial, const std::string& n) : ConfigValue(n)
    {
        this->value_ = initial;
        this->def_ = initial;
    }

    void draw() override
    {
        imgui_addons::checkbox(this->get_imgui_title(), &this->value_);
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

    [[nodiscard]] bool get_value() const
    {
        return this->value_;
    }

private:
    bool value_;
    bool def_;

    static std::string to_string(bool v)
    {
        std::ostringstream os("");
        os << std::boolalpha << v;
        return os.str();
    }

    static bool from_string(std::string string)
    {
        bool b1;
        std::istringstream is(string);
        is >> std::boolalpha >> b1;
        return b1;
    }
};
#pragma pack(pop)
