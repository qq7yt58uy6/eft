#pragma once
#include <string>

/**
 * Represents a config value of a module and its imgui representation.
 */
#pragma pack(push, 1)
class ConfigValue
{
public:
    virtual ~ConfigValue() = default;

    explicit ConfigValue(const std::string& name)
    {
        this->name_ = name;
    }

    /**
     * Register with the module
     * @param mod module name
     */
    void reg(const std::string& mod)
    {
        this->module_ = mod;
    }

    virtual void draw() = 0;

    virtual void set(const std::string& string) = 0;

    virtual std::string get() = 0;

    virtual std::string get_default() = 0;

    std::string get_imgui_title() const
    {
        return this->name_ + "##" + this->module_;
    }

private:
    std::string name_;
    std::string module_;
};
#pragma pack(pop)
