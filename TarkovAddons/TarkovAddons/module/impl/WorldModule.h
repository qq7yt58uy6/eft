#pragma once
#include "../game_state.h"
#include "../Module.h"
#include "../ModuleCategory.h"
#include "../../config/CheckboxValue.h"
#include "../../il2cpp/Il2CppObjectInstance.h"
#include "../../il2cpp/unity.h"

class WorldModule : Module
{
public:
    WorldModule() : Module("世界", Exploit)
    {
    }

    ActionRowValue* unlock_doors = conf(new ActionRowValue([&]
    {
        unlock_doors_impl();
    }, "解锁所有门"));
    bool queue_unlock_doors = false;

    void unlock_doors_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_unlock_doors = true;
    }

    void draw_overlay(ImDrawList* draw_list) override
    {
    }

    void application_update() override
    {
    }

    void gameworld_update(const Il2CppClass* game_world_class, Il2CppObjectInstance game_world_instance,
                          Il2CppObjectInstance main_player) override
    {
        if (queue_unlock_doors)
        {
            queue_unlock_doors = false;

            Il2CppObject* door_system_type = il2utils::get_system_type(game_world_class->image, "EFT.Interactive",
                                                                       "Door");
            size_t found_doors = 0;
            Il2CppObject** doors = unity::find_objects_of_type(door_system_type, &found_doors);
            for (size_t i = 0; i < found_doors; ++i)
            {
                Il2CppObjectInstance door_instance(doors[i]);
                // 2 = Shut
                door_instance.get_method<void(*)(Il2CppObject*, int)>("set_DoorState", 1)(
                    door_instance.get_instance(), 2);
                if (globals::verbose) logger::info("Unlocked Doors");
            }
        }
    }

    void init() override
    {
    }
};
