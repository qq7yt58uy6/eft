#pragma once
#include "../Module.h"
#include "../ModuleCategory.h"
#include "../../il2cpp/Il2CppObjectInstance.h"

class WeaponModule : Module
{
public:
    WeaponModule() : Module("武器", Exploit)
    {
    }

    CheckboxValue* no_jam = conf(new CheckboxValue(false, "无故障"));

    CheckboxValue* infinite_ammo = conf(new CheckboxValue(false, "无限子弹"));

    void draw_overlay(ImDrawList* draw_list) override
    {
    }

    void application_update() override
    {
    }

    void gameworld_update(const Il2CppClass* game_world_class, Il2CppObjectInstance game_world_instance,
                          Il2CppObjectInstance main_player) override
    {
        if (no_jam->get_value() || infinite_ammo->get_value())
        {
            Il2CppObject* hands_controller_object = main_player.get_method<Il2CppObject*(
                *)(Il2CppObject*)>("get_HandsController", 0)(main_player.get_instance());

            if (hands_controller_object && std::string(hands_controller_object->klass->name) == "FirearmController")
            {
                Il2CppObjectInstance firearm_controller(hands_controller_object);
                Il2CppObjectInstance weapon(
                    firearm_controller.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Item", 0)(
                        firearm_controller.get_instance()));
                Il2CppObjectInstance templt(
                    weapon.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Template", 0)(weapon.get_instance()));

                if (no_jam->get_value())
                {
                    templt.set_field("AllowOverheat", false);
                    templt.set_field("AllowJam", false);
                    templt.set_field("AllowMisfire", false);
                    templt.set_field("AllowSlide", false);
                    templt.set_field("AllowFeed", false);
                    templt.set_field("DurabilityBurnRatio", 0.0f);
                }

                if (infinite_ammo->get_value())
                {
                    Il2CppObject* mag_object = weapon.get_method<Il2CppObject*(*)(Il2CppObject*)>(
                        "GetCurrentMagazine", 0)(
                        weapon.get_instance());
                    if (mag_object)
                    {
                        Il2CppObjectInstance magazine(mag_object);
                        const int current_count = weapon.get_method<int(
                            *)(Il2CppObject*)>("GetCurrentMagazineCount", 0)(
                            weapon.get_instance());
                        const int max_count = weapon.get_method<int(*)(Il2CppObject*)>("GetMaxMagazineCount", 0)(
                            weapon.get_instance());

                        if (current_count != 0)
                        {
                            Il2CppObjectInstance first_real_ammo(magazine.get_method<Il2CppObject*(*)(Il2CppObject*)>(
                                "FirstRealAmmo", 0)(magazine.get_instance()));

                            const int max_stack_size = first_real_ammo.get_method<int(*)(Il2CppObject*)>(
                                "get_StackMaxSize", 0)(first_real_ammo.get_instance());

                            first_real_ammo.set_field("StackObjectsCount", min(max_count, max_stack_size));
                        }
                    }
                }
            }
        }
    }

    void init() override
    {
    }
};
