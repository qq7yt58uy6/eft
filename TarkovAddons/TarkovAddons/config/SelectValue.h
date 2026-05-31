#pragma once
#include <vector>

#include "ConfigValue.h"
#include "../gui/menu/imgui_addons.h"

#pragma pack(push, 1)
class SelectValue : public ConfigValue
{
public:
    SelectValue(int initial, const std::vector<const char*>& options, const std::string& n) : ConfigValue(n)
    {
        this->value_ = initial;
        this->def_ = initial;
        this->options = options;
    }

    void draw() override
    {
        imgui_addons::select(this->get_imgui_title(), &this->value_, this->options);
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

    std::string get_selected() const
    {
        return {this->options[this->value_]};
    }

    std::vector<const char*> get_options()
    {
        return this->options;
    }

    void set_options(const std::vector<const char*>& opts)
    {
        this->options = opts;
    }

private:
    int value_;
    int def_;
    std::vector<const char*> options;

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
