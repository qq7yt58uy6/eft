#pragma once
#include <imgui.h>
#include <vector>

#include "ModuleCategory.h"
#include "../config/ConfigValue.h"
#include "../il2cpp/Il2CppObjectInstance.h"
#include "../il2cpp/libil2cpp/il2cpp-api-types.h"

class Module
{
public:
    virtual ~Module()
    {
        for (const auto config_value : config_values)
        {
            delete config_value;
        }
    }

    Module(const std::string& name, const ModuleCategory category)
    {
        this->name = name;
        this->category = category;
    }

    std::string name;
    ModuleCategory category;

    virtual void draw_overlay(ImDrawList* draw_list) = 0;
    virtual void application_update() = 0;
    virtual void gameworld_update(const Il2CppClass* game_world_class, Il2CppObjectInstance game_world_instance,
                                  Il2CppObjectInstance main_player) = 0;
    virtual void init() = 0;

    std::vector<ConfigValue*> config_values = std::vector<ConfigValue*>();

    void apply(const std::function<void (ConfigValue*)>& function) const
    {
        for (auto config_value : config_values)
        {
            function(config_value);
        }
    }

    template <typename T>
    T* conf(T* value)
    {
        value->reg(this->name);
        config_values.push_back(value);
        return value;
    }
};
