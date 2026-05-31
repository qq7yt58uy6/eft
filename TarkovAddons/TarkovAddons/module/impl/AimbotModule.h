#pragma once
#include "../eft_api.h"
#include "../game_state.h"
#include "../Module.h"
#include "../../config/ColorpickerValue.h"
#include "../../config/FloatSliderValue.h"
#include "../../il2cpp/il2utils.h"
#include "../../il2cpp/Il2CppObjectInstance.h"
#include "../../il2cpp/unity.h"

static std::string me = "";
static unity::vector3 target_store = {};
using initiate_shot_sig = void(*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, unity::vector3, unity::vector3,
                                  unity::vector3, int, float);
static initiate_shot_sig o_initiate_shot;

inline void hk_initiate_shot(Il2CppObject* instance, Il2CppObject* weapon, Il2CppObject* ammo,
                             unity::vector3 shot_position, unity::vector3 shot_direction,
                             unity::vector3 fireport_position, int chamber_index, float overheat)
{
    if (!(target_store.x == 0 && target_store.y == 0 && target_store.z == 0)) // NOLINT(clang-diagnostic-float-equal)
    {
        // validate this is us shooting
        Il2CppObjectInstance hands_controller_instance(instance);
        Il2CppObjectInstance player(hands_controller_instance.get_field<Il2CppObject*>("_player"));

        const auto get_profile = player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Profile", 0);
        Il2CppObjectInstance profile(get_profile(player.get_instance()));

        const std::string nickname = eft_api::to_english(il2utils::conv_wstring(
            profile.get_method<Il2CppString*(*)(Il2CppObject*)>("get_Nickname", 0)(
                profile.get_instance())));

        if (nickname == me)
        {
            // set shot position to enemy head and make it go down
            shot_position = target_store;
            shot_direction = {0, -1, 0};
            fireport_position = target_store;
        }
    }

    o_initiate_shot(instance, weapon, ammo, shot_position, shot_direction, fireport_position, chamber_index, overheat);
}

class AimbotModule : Module
{
public:
    AimbotModule() : Module("自瞄菜单", Aimbot)
    {
    }

    CheckboxValue* magic_bullet = conf(new CheckboxValue(false, "魔法子弹"));
    CheckboxValue* show_target = conf(new CheckboxValue(true, "显示追踪点"));
    FloatSliderValue* target_size = conf(new FloatSliderValue(3, 1, 10, "追踪点大小"));
    ColorpickerValue* target_color = conf(new ColorpickerValue({1, 0, 1, 1}, "追踪点颜色"));

    void draw_overlay(ImDrawList* draw_list) override
    {
        // no aimbot outside of raid
        if (!game_state::is_in_raid) return;

        // find cameras
        Il2CppObject* cam = unity::get_current_camera();

        // show target
        if (magic_bullet->get_value() && show_target->get_value())
        {
            if (!(target_store.x == 0 && target_store.y == 0)) // NOLINT(clang-diagnostic-float-equal)
                if (unity::vector3 screen_pos = eft_api::world_to_screen(cam, target_store, game_state::current_zoom);
                    eft_api::is_visible(screen_pos))
                {
                    draw_list->AddCircleFilled({screen_pos.x, screen_pos.y},
                                               target_size->get_value() * menu::get_scale_factor(),
                                               ImGui::GetColorU32(target_color->get_value()));
                }
        }
    }

    void application_update() override
    {
    }

    void gameworld_update(const Il2CppClass* game_world_class, Il2CppObjectInstance game_world_instance,
                          Il2CppObjectInstance main_player) override
    {
        const auto get_profile = main_player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Profile", 0);
        Il2CppObjectInstance main_profile(get_profile(main_player.get_instance()));

        const std::string main_nickname = eft_api::to_english(il2utils::conv_wstring(
            main_profile.get_method<Il2CppString*(*)(Il2CppObject*)>("get_Nickname", 0)(
                main_profile.get_instance())));
        me = main_nickname;

        // player esp
        if (magic_bullet->get_value())
        {
            // find cameras
            Il2CppObject* cam = unity::get_current_camera();
            // Il2CppObject* scope_cam = unity::get_named_camera("BaseOpticCamera(Clone)");

            const unity::vector3 screen_center = {menu::get_width() / 2, menu::get_height() / 2, 0};

            float closest = 999999999.0f;
            unity::vector3 closest_pos = {};

            Il2CppArray* alive_players = game_world_instance.get_field<unity::list*>("AllAlivePlayersList")->
                                                             m_p_list_array;
            for (size_t i = 0; i < alive_players->max_length; ++i)
            {
                Il2CppObject* player_object =
                    reinterpret_cast<Il2CppObject**>(&alive_players->data)[i];
                if (!player_object) continue;
                Il2CppObjectInstance player_instance(player_object);

                Il2CppObjectInstance profile_instance(get_profile(player_instance.get_instance()));

                std::string nickname = eft_api::to_english(il2utils::conv_wstring(
                    profile_instance.get_method<Il2CppString*(*)(Il2CppObject*)>("get_Nickname", 0)(
                        profile_instance.get_instance())));

                // skip ourselves
                if (nickname == main_nickname) continue;

                // get head position
                Il2CppObjectInstance player_body(player_instance.get_field<Il2CppObject*>("_playerBody"));
                Il2CppObjectInstance player_bones(player_body.get_field<Il2CppObject*>("PlayerBones"));
                Il2CppObjectInstance bifacial_transform_head(player_bones.get_field<Il2CppObject*>("Head"));
                const unity::vector3 head_pos = bifacial_transform_head.get_method<unity::vector3(
                    *)(Il2CppObject*)>("get_position", 0)(bifacial_transform_head.get_instance());

                unity::vector3 screen_pos = eft_api::world_to_screen(cam, head_pos, game_state::current_zoom);
                if (!eft_api::is_visible(screen_pos)) continue;
                screen_pos.z = 0;

                if (const float dist = screen_pos.distance(screen_center); dist < closest)
                {
                    closest_pos = head_pos;
                    closest = dist;
                }
            }

            target_store = closest_pos;
        }
        else
        {
            target_store = {};
        }
    }

    void init() override
    {
        Il2CppImage* image = il2utils::resolve_image("Assembly-CSharp.dll");
        const Il2CppClass* player_class = il2utils::resolve_class(image, "EFT", "Player");
        const Il2CppClass* firearm_controller_class = il2utils::resolve_class_nested(player_class, "FirearmController");
        il2utils::hook_method(firearm_controller_class, "InitiateShot", 7, hk_initiate_shot, &o_initiate_shot);
    }
};
