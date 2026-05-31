#pragma once
#include "../game_state.h"
#include "../Module.h"
#include "../ModuleCategory.h"
#include "../../config/CheckboxValue.h"
#include "../../config/FloatSliderValue.h"
#include "../../il2cpp/Il2CppObjectInstance.h"

static bool do_speed;
static int speed_value;

static bool should_survive;

using fixed_update_sig = void(*)(Il2CppObject*);
static fixed_update_sig o_player_camera_controller_fixed_update = nullptr;

inline void hk_player_camera_controller_fixed_update(Il2CppObject* instance)
{
    if (do_speed)
    {
        Il2CppObjectInstance controller_instance(instance);
        Il2CppObjectInstance main_player(controller_instance.get_method<Il2CppObject*(
            *)(Il2CppObject*)>("get_Player", 0)(instance));
        Il2CppObjectInstance character_controller(main_player.get_field<Il2CppObject*>("_characterController"));
        unity::vector3 velocity = character_controller.get_method<unity::vector3(*)(Il2CppObject*)>("get_velocity", 0)(
            character_controller.get_instance());
        velocity.x /= 100;
        velocity.y = 0;
        velocity.z /= 100;
        for (int i = 0; i < speed_value; ++i)
        {
            character_controller.get_method<int(*)(Il2CppObject*, unity::vector3, float)>("Move", 2)(
                character_controller.get_instance(), velocity, 100.0f);
        }
    }

    o_player_camera_controller_fixed_update(instance);
}

using local_game_stop_sig = void(*)(Il2CppObject*, Il2CppString*, int, Il2CppString*, float);

static local_game_stop_sig o_local_game_stop;

inline void hk_local_game_stop(Il2CppObject* instance, Il2CppString* profile_id, int exit_status,
                               Il2CppString* exit_name, float delay)
{
    if (should_survive) exit_status = 0; // Survived

    o_local_game_stop(instance, profile_id, exit_status, exit_name, delay);
}

class PlayerModule : Module
{
public:
    PlayerModule() : Module("玩家", Exploit)
    {
    }

    CheckboxValue* god_mode = conf(new CheckboxValue(false, "无敌"));
    CheckboxValue* infinite_stamina = conf(new CheckboxValue(false, "无限耐力"));
    CheckboxValue* no_hunger_thirst = conf(new CheckboxValue(false, "满状态"));
    CheckboxValue* instant_search = conf(new CheckboxValue(false, "秒搜"));

    SelectValue* exp_type = conf(new SelectValue(0, {
                                                     "ExpDoorBreached", "ExpItemLooting", "ExpDamage", "ExpHeal",
                                                     "ExpExamine", "ExpKillBase", "ExpItemLooting"
                                                 }, "经验种类"));
    IntSliderValue* exp_amount = conf(new IntSliderValue(10000, 0, 500000, "经验数值"));
    ActionRowValue* add_exp = conf(new ActionRowValue([&]
    {
        add_exp_impl();
    }, "添加经验"));

    SelectValue* fence_rep_source = conf(new SelectValue(0, {
                                                             "Boss Help", "Scav Help", "Scav Kill", "Boss Kill",
                                                             "Traitor Kill", "Exit Standing"
                                                         }, "好感来源"));
    FloatSliderValue* fence_rep_amount = conf(new FloatSliderValue(0.1f, 0, 5, "好感数值"));
    ActionRowValue* add_fence_rep = conf(new ActionRowValue([&]
    {
        add_fence_rep_impl();
    }, "增加好感"));

    SelectValue* trader_select = conf(new SelectValue(0, {
                                                          "LIGHT_KEEPER_TRADER", "RAGMAN_TRADER",
                                                          "ARENA_MANAGER_TRADER", "PEACEKEEPER", "SKIER",
                                                          "PRAPOR_TRADER", "JAEGER_TRADER", "MECHANIC_TRADER",
                                                          "TERAPEVT_TRADER", "BTR_TRADER"
                                                      }, "商人"));
    FloatSliderValue* trader_rep_amount = conf(new FloatSliderValue(0.1f, 0, 5, "商人好感数值"));
    ActionRowValue* add_trader_rep = conf(new ActionRowValue([&]
    {
        add_trader_rep_impl();
    }, "增加商人好感"));

    SelectValue* skill_select = conf(new SelectValue(0, {
                                                         "Pistol", "Revolver", "SMG", "Assault", "Shotgun", "Sniper",
                                                         "LMG", "HMG", "Launcher", "AttachedLauncher", "Misc", "Melee",
                                                         "DMR", "Throwing", "AimDrills", "RecoilControl",
                                                         "TroubleShooting", "Perception", "Attention", "Intellect",
                                                         "Immunity", "Charisma", "Memory", "Endurance", "Strength",
                                                         "Vitality", "Health", "Metabolism", "StressResistance",
                                                         "Sniping", "CovertMovement", "ProneMovement", "LightVests",
                                                         "HeavyVests", "WeaponModding", "AdvancedModding",
                                                         "Lockpicking", "Surgery", "Search", "WeaponTreatment",
                                                         "MagDrills", "Freetrading", "Barter", "Crafting",
                                                         "HideoutManagement", "BearAssaultoperations", "BearAuthority",
                                                         "BearAksystems", "BearHeavycaliber", "BearRawpower",
                                                         "UsecArsystems", "UsecDeepweaponmodding",
                                                         "UsecLongrangeoptics", "UsecNegotiations", "UsecTactics"
                                                     }, "技能"));
    IntSliderValue* skill_level = conf(new IntSliderValue(51, 0, 51, "技能等级"));
    ActionRowValue* set_skill = conf(new ActionRowValue([&]
    {
        set_skill_impl();
    }, "修改技能"));

    CheckboxValue* speed_hack = conf(new CheckboxValue(false, "变速功能"));
    IntSliderValue* speed = conf(new IntSliderValue(2, 1, 10, "移动速度"));

    CheckboxValue* always_survive = conf(new CheckboxValue(false, "永久存活"));

    bool queue_add_exp = false;
    bool queue_add_fence_rep = false;
    bool queue_add_trader_rep = false;
    bool queue_set_skill = false;

    void add_exp_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_add_exp = true;
    }

    void add_fence_rep_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_add_fence_rep = true;
    }

    void add_trader_rep_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_add_trader_rep = true;
    }

    void set_skill_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_set_skill = true;
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
        const auto get_profile = main_player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Profile", 0);
        Il2CppObjectInstance main_profile(get_profile(main_player.get_instance()));

        if (infinite_stamina->get_value())
        {
            Il2CppObjectInstance physical(main_player.get_field<Il2CppObject*>("Physical"));

            Il2CppObjectInstance stamina(physical.get_field<Il2CppObject*>("Stamina"));
            stamina.set_field("Current", 100.0f);

            Il2CppObjectInstance stamina_hands(physical.get_field<Il2CppObject*>("HandsStamina"));
            stamina_hands.set_field("Current", 100.0f);

            Il2CppObjectInstance oxygen(physical.get_field<Il2CppObject*>("Oxygen"));
            oxygen.set_field("Current", 100.0f);
        }

        Il2CppObjectInstance health_controller(
            main_player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_ActiveHealthController", 0)(
                main_player.get_instance()));
        if (god_mode->get_value())
        {
            health_controller.get_method<void(*)(Il2CppObject*, float)>("SetDamageCoeff", 1)(
                health_controller.get_instance(), -1.0f);
            health_controller.get_method<void(*)(Il2CppObject*, int)>("RemoveNegativeEffects", 1)(
                health_controller.get_instance(), 7); // 7 = Common
            health_controller.get_method<void(*)(Il2CppObject*)>("RestoreFullHealth", 0)(
                health_controller.get_instance());
            health_controller.set_field("_fallSafeHeight", 99999999.0f);
        }

        if (no_hunger_thirst->get_value())
        {
            health_controller.get_method<void(*)(Il2CppObject*, float)>("ChangeEnergy", 1)(
                health_controller.get_instance(), 100.0f);
            health_controller.get_method<void(*)(Il2CppObject*, float)>("ChangeHydration", 1)(
                health_controller.get_instance(), 100.0f);
        }

        Il2CppObjectInstance skill_manager(
            main_player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Skills", 0)(main_player.get_instance()));
        if (instant_search->get_value())
        {
            Il2CppObjectInstance attention_elite_lucky_search(
                skill_manager.get_field<Il2CppObject*>("AttentionEliteLuckySearch"));

            attention_elite_lucky_search.set_field("Value", 100000.0f);
        }

        if (queue_add_exp)
        {
            queue_add_exp = false;
            Il2CppObjectInstance eft_stats(
                main_profile.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_EftStats", 0)(
                    main_profile.get_instance()));

            Il2CppObjectInstance session_counters(eft_stats.get_field<Il2CppObject*>("SessionCounters"));

            const Il2CppImage* assembly_csharp = il2utils::resolve_image("Assembly-CSharp.dll");
            const Il2CppClass* predefined_counters = il2utils::resolve_class(assembly_csharp, "EFT.Counters",
                                                                             "PredefinedCounters");
            const auto reason = il2utils::get_static_field<Il2CppObject
                *>(predefined_counters, exp_type->get_selected().c_str());

            session_counters.get_method<void(*)(Il2CppObject*, int, Il2CppObject*)>("AddInt", 2)(
                session_counters.get_instance(), exp_amount->get_value(), reason);

            if (globals::verbose) logger::info("Added EXP");
        }

        if (queue_add_fence_rep)
        {
            queue_add_fence_rep = false;
            Il2CppObjectInstance loyalty_data(
                main_player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Loyalty", 0)(
                    main_player.get_instance()));

            loyalty_data.get_method<void(*)(Il2CppObject*, double, int)>("KillAsSavage", 2)(
                loyalty_data.get_instance(), fence_rep_amount->get_value(), fence_rep_source->get_value());

            if (globals::verbose) logger::info("Added Fence Rep");
        }

        if (queue_add_trader_rep)
        {
            queue_add_trader_rep = false;
            std::string trader_id_key = trader_select->get_selected() + "_ID";

            const Il2CppClass* profile_class = il2utils::resolve_class(game_world_class->image, "EFT", "Profile");
            const Il2CppClass* trader_info_class = il2utils::resolve_class_nested(profile_class, "TraderInfo");
            const auto trader_id = il2utils::get_static_field<Il2CppString*>(trader_info_class, trader_id_key.c_str());
            const auto try_get_trader_info = il2utils::get_method<bool(
                *)(Il2CppObject*, Il2CppString*, Il2CppObject**)>(
                profile_class, "TryGetTraderInfo", 2);

            Il2CppObject* trader_info = nullptr;
            if (try_get_trader_info(main_profile.get_instance(), trader_id, &trader_info) && trader_info)
            {
                Il2CppObjectInstance trader_info_instance(trader_info);

                const double standing = trader_info_instance.get_method<double(*)(Il2CppObject*)>("get_Standing", 0)(
                    trader_info);

                trader_info_instance.get_method<void(*)(Il2CppObject*, double)>("SetStanding", 1)(
                    trader_info, trader_rep_amount->get_value() + standing);
            }

            if (globals::verbose) logger::info("Added Trader Rep");
        }

        if (queue_set_skill)
        {
            queue_set_skill = false;

            Il2CppObjectInstance skill_controller(
                main_player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Skills", 0)(main_player.get_instance()));

            if (const auto skill_ptr = skill_controller.get_field<Il2CppObject*>(skill_select->get_selected().c_str()))
            {
                Il2CppObjectInstance skill_instance(skill_ptr);

                skill_instance.get_method<void(*)(Il2CppObject*, int)>("SetLevel", 1)(
                    skill_instance.get_instance(), skill_level->get_value());

                if (globals::verbose)
                    logger::info(
                        "Set Skill: " + skill_select->get_selected() + " Level: " +
                        std::to_string(skill_level->get_value()));
            }
            else
            {
                if (globals::verbose) logger::warn("Skill not found: " + skill_select->get_selected());
            }
        }

        do_speed = speed_hack->get_value();
        speed_value = speed->get_value();

        should_survive = always_survive->get_value();
    }

    void init() override
    {
        const Il2CppImage* image = il2utils::resolve_image("Assembly-CSharp.dll");

        // need a fixed update here so speed hack is consistent, this is a good one i found with access to player
        const Il2CppClass* player_camera_controller_class = il2utils::resolve_class(
            image, "EFT.CameraControl", "PlayerCameraController");
        il2utils::hook_method(player_camera_controller_class, "FixedUpdate", 0,
                              hk_player_camera_controller_fixed_update,
                              &o_player_camera_controller_fixed_update);

        // local game stop
        const Il2CppClass* local_game = il2utils::resolve_class(image, "EFT", "LocalGame");
        il2utils::hook_method(local_game, "Stop", 4, hk_local_game_stop, &o_local_game_stop);
    }
};
