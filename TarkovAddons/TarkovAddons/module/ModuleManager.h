#pragma once
#include <functional>

#include "Module.h"
#include "impl/AimbotModule.h"
#include "impl/ChamsModule.h"
#include "impl/ConfigModule.h"
#include "impl/EspModule.h"
#include "impl/ItemModule.h"
#include "impl/PlayerModule.h"
#include "impl/QuestModule.h"
#include "impl/WatermarkModule.h"
#include "impl/WeaponModule.h"
#include "impl/WorldModule.h"

class ModuleManager;
inline static ModuleManager* instance = nullptr;

class ModuleManager
{
public:
    ~ModuleManager()
    {
        for (auto module : modules)
        {
            delete static_cast<Module*>(module);
        }
    }

    ModuleManager()
    = default;

    void init() const
    {
        for (auto module : modules)
        {
            static_cast<Module*>(module)->init();
        }
    }

    std::vector<PVOID> modules = std::vector<PVOID>();

    void apply(const std::function<void (Module*)>& function) const
    {
        for (auto module : modules)
        {
            function(static_cast<Module*>(module));
        }
    }

    template <typename T>
    T* module(T* value)
    {
        modules.push_back(static_cast<PVOID>(value));
        return value;
    }

    std::vector<Module*> get_modules() const
    {
        auto res = std::vector<Module*>();
        for (auto module : modules)
        {
            res.push_back(static_cast<Module*>(module));
        }
        return res;
    }

    // register modules here

    // aimbot
    AimbotModule* aimbot_module = module(new AimbotModule());

    // visuals
    EspModule* esp_module = module(new EspModule());
    ChamsModule* chams_module = module(new ChamsModule());

    // exploit
    PlayerModule* player_module = module(new PlayerModule());
    WorldModule* world_module = module(new WorldModule());
    WeaponModule* weapon_module = module(new WeaponModule());
    ItemModule* item_module = module(new ItemModule());
    QuestModule* quest_module = module(new QuestModule());

    // misc
    WatermarkModule* watermark_module = module(new WatermarkModule());

    // this has to get instantiated last
    ConfigModule* config_module = module(new ConfigModule(get_modules()));

    static ModuleManager* get_instance()
    {
        if (!instance)
        {
            instance = new ModuleManager();
        }
        return instance;
    }
};
