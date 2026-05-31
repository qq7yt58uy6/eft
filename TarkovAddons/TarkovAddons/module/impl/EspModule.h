#pragma once
#include "../eft_api.h"
#include "../game_state.h"
#include "../../config/IntSliderValue.h"
#include "../../il2cpp/il2utils.h"
#include "../../il2cpp/Il2CppObjectInstance.h"
#include "../../il2cpp/unity.h"

class EspModule : Module
{
public:
    EspModule() : Module("透视", Visuals)
    {
    }

    CheckboxValue* extracts = conf(new CheckboxValue(false, "撤离点"));
    ColorpickerValue* extracts_color = conf(new ColorpickerValue({ 0, 1, 0, 1 }, "撤离点颜色"));

    CheckboxValue* transits = conf(new CheckboxValue(false, "转移点"));
    ColorpickerValue* transits_color = conf(new ColorpickerValue({ 1, 1, 0, 1 }, "转移点颜色"));

    CheckboxValue* players = conf(new CheckboxValue(false, "人物"));
    ColorpickerValue* player_color = conf(new ColorpickerValue({ 1, 0, 0, 1 }, "人物颜色"));
    CheckboxValue* npc = conf(new CheckboxValue(false, "AI"));
    ColorpickerValue* npc_color = conf(new ColorpickerValue({ 1, 1, 0, 1 }, "AI颜色"));

    CheckboxValue* loot_esp = conf(new CheckboxValue(false, "物品"));
    ColorpickerValue* loot_color = conf(new ColorpickerValue({ 0, 1, 1, 1 }, "物品颜色"));
    FloatSliderValue* loot_distance = conf(new FloatSliderValue(50.f, 1.f, 500.f, "物品距离"));

    CheckboxValue* container_esp = conf(new CheckboxValue(false, "容器"));
    ColorpickerValue* container_color = conf(new ColorpickerValue({ 1, 0.5f, 0, 1 }, "容器颜色"));
    FloatSliderValue* container_distance = conf(new FloatSliderValue(100.f, 1.f, 500.f, "容器距离"));

    IntSliderValue* font_size = conf(new IntSliderValue(12, 8, 36, "字体大小"));

    enum class player_type : char
    {
        pmc,
        npc,
    };

    struct exfil
    {
        std::string name;
        int status;
        unity::vector3 pos3d;
    };

    struct transit
    {
        std::string name;
        unity::vector3 pos3d;
    };

    struct player
    {
        std::string name;
        bool pmc;
        unity::vector3 pos3d;
    };

    struct loot_entry
    {
        std::string name;
        unity::vector3 pos3d;
        bool is_container;
    };

    struct loot_scan_state
    {
        std::vector<Il2CppObject*> loot_objects;
        std::vector<Il2CppObject*> container_objects;

        std::vector<loot_entry> loot_results;
        std::vector<loot_entry> container_results;

        size_t loot_index = 0;
        size_t container_index = 0;

        ULONGLONG last_discovery = 0;
    };

    inline static loot_scan_state loot_scan{};

    unity::vector3 my_pos = {};

    std::vector<exfil> exfil_store = std::vector<exfil>();
    std::vector<transit> transits_store = std::vector<transit>();
    std::vector<player> player_store = std::vector<player>();
    std::vector<loot_entry> loot_store;

    struct cached_container
    {
        Il2CppObject* obj;
        std::string name;
    };

    inline static std::vector<cached_container> cached_containers{};
    inline static ULONGLONG cached_containers_last_refresh = 0;

private:
    ULONGLONG loot_last_refresh = 0;
public:


    void draw_overlay(ImDrawList* draw_list) override
    {
        // no esp outside of raid
        if (!game_state::is_in_raid) return;

        // find cameras
        Il2CppObject* cam = unity::get_current_camera();
        // Il2CppObject* scope_cam = unity::get_named_camera("BaseOpticCamera(Clone)");

        // extract esp
        if (extracts->get_value())
        {
            for (const auto& [name, status, pos3d] : exfil_store)
            {
                unity::vector3 screen_pos = eft_api::world_to_screen(cam, pos3d, game_state::current_zoom);
                if (!eft_api::is_visible(screen_pos)) continue;
                const float distance = my_pos.distance(pos3d);

                std::string status_string;
                switch (status)
                {
                case 8:
                    status_string = "Hidden";
                    break;
                case 7:
                    status_string = "Inactive";
                    break;
                case 6:
                    status_string = "Pending";
                    break;
                case 5:
                case 4:
                    status_string = "Open";
                    break;
                case 2:
                    status_string = "Available";
                    break;
                case 1:
                    status_string = "Closed";
                    break;
                default:
                    status_string = "Unknown";
                    break;
                }

                std::string text = std::string("[").append(std::to_string(static_cast<int>(distance))).append("m] ").
                    append(name).append(" (").append(status_string).append(")");

                imgui_addons::centered_text(draw_list, menu::get_hud_font(),
                    static_cast<float>(font_size->get_value()) * menu::get_scale_factor(),
                    { screen_pos.x, screen_pos.y }, extracts_color->get_value(), text);
            }
        }

        // transit esp
        if (transits->get_value())
        {
            for (const auto& [name, pos3d] : transits_store)
            {
                unity::vector3 screen_pos = eft_api::world_to_screen(cam, pos3d, game_state::current_zoom);
                if (!eft_api::is_visible(screen_pos)) continue;
                const float distance = my_pos.distance(pos3d);
                std::string text = std::string("[").append(std::to_string(static_cast<int>(distance))).append("m] ").
                    append(name);

                imgui_addons::centered_text(draw_list, menu::get_hud_font(),
                    static_cast<float>(font_size->get_value()) * menu::get_scale_factor(),
                    { screen_pos.x, screen_pos.y }, transits_color->get_value(), text);
            }
        }

        // player esp
        if (players->get_value())
        {
            for (const auto& [name, pmc, pos3d] : player_store)
            {
                unity::vector3 screen_pos = eft_api::world_to_screen(cam, pos3d, game_state::current_zoom);
                if (!eft_api::is_visible(screen_pos)) continue;
                const float distance = my_pos.distance(pos3d);
                std::string text = std::string("[").append(std::to_string(static_cast<int>(distance))).append("m] ").
                    append(name);

                switch (pmc) // NOLINT(clang-diagnostic-switch-bool)
                {
                case true:
                    if (!players->get_value()) break;
                    imgui_addons::centered_text(draw_list, menu::get_hud_font(),
                        static_cast<float>(font_size->get_value()) * menu::get_scale_factor(),
                        { screen_pos.x, screen_pos.y + 10 * menu::get_scale_factor() },
                        player_color->get_value(), text);
                    break;
                case false:
                    if (!npc->get_value()) break;
                    imgui_addons::centered_text(draw_list, menu::get_hud_font(),
                        static_cast<float>(font_size->get_value()) * menu::get_scale_factor(),
                        { screen_pos.x, screen_pos.y + 10 * menu::get_scale_factor() },
                        npc_color->get_value(), text);
                    break;
                }
            }
        }

        const bool show_loot = loot_esp->get_value();
        const bool show_containers = container_esp->get_value();
        const float loot_max = loot_distance->get_value();
        const float container_max = container_distance->get_value();

        if (show_loot || show_containers)
        {
            for (const auto& l : loot_store)
            {
                if (l.is_container && !show_containers) continue;
                if (!l.is_container && !show_loot) continue;

                const float dx = my_pos.x - l.pos3d.x;
                const float dy = my_pos.y - l.pos3d.y;
                const float dz = my_pos.z - l.pos3d.z;
                const float dist_sq = dx * dx + dy * dy + dz * dz;

                const float max_dist = l.is_container ? container_max : loot_max;
                if (dist_sq > max_dist * max_dist) continue;

                unity::vector3 screen_pos = eft_api::world_to_screen(cam, l.pos3d, game_state::current_zoom);
                if (!eft_api::is_visible(screen_pos)) continue;

                const int distance = static_cast<int>(sqrtf(dist_sq));

                char buf[128];
                snprintf(buf, sizeof(buf), "[%dm] %s", distance, l.name.c_str());

                ImVec4 color = l.is_container ? container_color->get_value() : loot_color->get_value();

                imgui_addons::centered_text(
                    draw_list,
                    ImVec2((float)screen_pos.x, (float)screen_pos.y),
                    color,
                    buf
                );
            }
        }
    }
    std::string utf16_to_utf8(const wchar_t* utf16_str)
    {
        if (!utf16_str || wcslen(utf16_str) == 0) return "";

        int size = WideCharToMultiByte(CP_UTF8, 0, utf16_str, -1, nullptr, 0, nullptr, nullptr);
        std::string utf8_str(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, utf16_str, -1, &utf8_str[0], size, nullptr, nullptr);
        utf8_str.resize(size - 1); // 移除末尾\0
        return utf8_str;
    }
    void application_update() override
    {
    }

    void gameworld_update(const Il2CppClass* game_world_class, Il2CppObjectInstance game_world_instance,
        Il2CppObjectInstance main_player) override
    {
        const auto main_player_game_object = unity::component_get_game_object(main_player.get_instance());
        const auto main_player_transform = unity::gameobject_get_transform(main_player_game_object);
        my_pos = unity::transform_get_pos(main_player_transform);

        const auto get_profile = main_player.get_method<Il2CppObject * (*)(Il2CppObject*)>("get_Profile", 0);
        Il2CppObjectInstance main_profile(get_profile(main_player.get_instance()));

        const std::string main_nickname = eft_api::to_english(il2utils::conv_wstring(
            main_profile.get_method<Il2CppString * (*)(Il2CppObject*)>("get_Nickname", 0)(
                main_profile.get_instance())));

        // extract esp
        if (extracts->get_value())
        {
            Il2CppObjectInstance exfiltration_controller(
                game_world_instance.get_field<Il2CppObject*>(
                    "<ExfiltrationController>k__BackingField"));

            const auto exfils = exfiltration_controller.get_method<Il2CppArray * (
                *)(Il2CppObject*, Il2CppObject*)>("EligiblePoints", 1)(exfiltration_controller.get_instance(),
                    main_profile.get_instance());

            auto new_exfils = std::vector<exfil>();
            for (size_t i = 0; i < exfils->max_length; ++i)
            {
                Il2CppObject* exfil = reinterpret_cast<Il2CppObject**>(&exfils->data)[i];
                if (!exfil) continue;
                Il2CppObjectInstance exfiltration_point(exfil);

                const int status = exfiltration_point.get_field<int>("_status");

                Il2CppObjectInstance exfil_settings(exfiltration_point.get_field<Il2CppObject*>("Settings"));

                const std::string name = il2utils::conv_string(exfil_settings.get_field<Il2CppString*>("Name"));
               
                Il2CppObject* game_object = unity::component_get_game_object(exfiltration_point.get_instance());
                Il2CppObject* tranform = unity::gameobject_get_transform(game_object);

                new_exfils.push_back({
                     name, status, unity::transform_get_pos(tranform)
                    });
            }

            exfil_store = new_exfils;
        }

        // transit esp
        if (transits->get_value())
        {
            Il2CppObjectInstance transit_controller(
                game_world_instance.get_field<Il2CppObject*>("<TransitController>k__BackingField"));

            const auto transits_by_id = transit_controller.get_field<unity::dict<int, Il2CppObject*>*>("pointsById");

            auto new_transits = std::vector<transit>();

            for (int i = 0; i < transits_by_id->m_i_count; ++i)
            {
                Il2CppObjectInstance transit(transits_by_id->get_value_by_index(i));

                Il2CppObjectInstance parameters(transit.get_field<Il2CppObject*>("parameters"));

                const std::string location = il2utils::conv_string(parameters.get_field<Il2CppString*>("location"));

                Il2CppObject* transit_game_object = unity::component_get_game_object(transit.get_instance());
                Il2CppObject* transit_transform = unity::gameobject_get_transform(transit_game_object);

                new_transits.push_back({
                    location, unity::transform_get_pos(transit_transform)
                    });
            }

            transits_store = new_transits;
        }

        // player esp
        auto new_players = std::vector<player>();
        if (players->get_value() || npc->get_value())
        {
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
                    profile_instance.get_method<Il2CppString * (*)(Il2CppObject*)>("get_Nickname", 0)(
                        profile_instance.get_instance())));

                // skip ourselves
                if (nickname == main_nickname) continue;

                Il2CppObject* player_game_object = unity::component_get_game_object(player_instance.get_instance());
                Il2CppObject* player_transform = unity::gameobject_get_transform(player_game_object);

                // check if player has a dogtag to see if it's a pmc, yes this is actually the best way i found to do this
                Il2CppObjectInstance inventory_controller(
                    player_instance.get_field<Il2CppObject*>("_inventoryController"));

                Il2CppObjectInstance inventory(
                    inventory_controller.get_method<Il2CppObject * (*)(Il2CppObject*)>("get_Inventory", 0)(
                        inventory_controller.get_instance()));

                Il2CppObjectInstance equipment(
                    inventory.get_field<Il2CppObject*>("Equipment"));

                // 13 = Dogtag
                Il2CppObjectInstance equipment_slot(
                    equipment.get_method<Il2CppObject * (*)(Il2CppObject*, int)>("GetSlot", 1)(
                        equipment.get_instance(), 13));

                Il2CppObject* item_ptr = equipment_slot.get_method<Il2CppObject * (*)(Il2CppObject*)>(
                    "get_ContainedItem", 0)(
                        equipment_slot.get_instance());

                new_players.push_back({
                    nickname, item_ptr != nullptr, unity::transform_get_pos(player_transform)
                    });
            }

            player_store = new_players;
        }

        if (loot_esp->get_value() || container_esp->get_value())
        {
            std::vector<loot_entry> new_loot;

            const float min_dist = 2.0f;
            const float min_dist_sq = min_dist * min_dist;

            if (loot_esp->get_value())
            {
                static auto* assembly_csharp = il2utils::resolve_image("Assembly-CSharp.dll");
                static auto* localization_extensions_class =
                    il2utils::resolve_class(assembly_csharp, "EFT", "LocalizationExtensions");

                static auto localized_short_name =
                    il2utils::get_method<Il2CppString * (*)(Il2CppObject*)>(
                        localization_extensions_class,
                        "LocalizedShortName",
                        1
                    );

                Il2CppObject* loot_hydra_obj = game_world_instance.get_field<Il2CppObject*>("LootItems");
                if (loot_hydra_obj)
                {
                    Il2CppObjectInstance loot_hydra(loot_hydra_obj);
                    unity::list* loot_list = loot_hydra.get_field<unity::list*>("_iteration");

                    if (loot_list && loot_list->m_p_list_array)
                    {
                        Il2CppArray* loot_array = loot_list->m_p_list_array;
                        const int count = static_cast<int>(loot_array->max_length);

                        if (count > 0)
                        {
                            new_loot.reserve(static_cast<size_t>(count));
                        }

                        const float loot_max_dist = loot_distance->get_value();
                        const float loot_max_dist_sq = loot_max_dist * loot_max_dist;

                        for (int i = 0; i < count; ++i)
                        {
                            Il2CppObject* loot_obj =
                                reinterpret_cast<Il2CppObject**>(&loot_array->data)[i];
                            if (!loot_obj) continue;

                            Il2CppObject* game_object = unity::component_get_game_object(loot_obj);
                            if (!game_object) continue;

                            Il2CppObject* transform = unity::gameobject_get_transform(game_object);
                            if (!transform) continue;

                            unity::vector3 pos = unity::transform_get_pos(transform);

                            const float dx = my_pos.x - pos.x;
                            const float dy = my_pos.y - pos.y;
                            const float dz = my_pos.z - pos.z;
                            const float dist_sq = dx * dx + dy * dy + dz * dz;

                            if (dist_sq < min_dist_sq || dist_sq > loot_max_dist_sq) continue;

                            Il2CppObjectInstance loot_instance(loot_obj);

                            std::string name = "Item";
                            Il2CppObject* item_obj = loot_instance.get_field<Il2CppObject*>("_item");
                            if (item_obj && localized_short_name)
                            {
                                Il2CppString* name_str = localized_short_name(item_obj);
                               
                                if (name_str)
                                {
                                    std::wstring wname = il2utils::conv_wstring(name_str);
                                    name = utf16_to_utf8(wname.c_str());
                                   
                                    if (name.empty())
                                        name = "Item";
                                }
                            }

                            new_loot.push_back({
                                std::move(name), pos, false
                                });
                        }
                    }
                }
            }

            if (container_esp->get_value())
            {
                ULONGLONG now = GetTickCount64();

                if (now - cached_containers_last_refresh >= 15000 || cached_containers.empty())
                {
                    cached_containers_last_refresh = now;
                    cached_containers.clear();

                    unity::list* loot_list = game_world_instance.get_field<unity::list*>("LootList");
                    if (loot_list && loot_list->m_p_list_array)
                    {
                        Il2CppArray* loot_array = loot_list->m_p_list_array;
                        const int count = static_cast<int>(loot_array->max_length);
                        for (int i = 0; i < count; ++i)
                        {
                            Il2CppObject* obj =
                                reinterpret_cast<Il2CppObject**>(&loot_array->data)[i];
                            if (!obj) continue;

                            const char* class_name = (obj->klass && obj->klass->name) ? obj->klass->name : "";
                            if (!class_name || !class_name[0]) continue;

                            const bool looks_like_container =
                                strstr(class_name, "LootableContainer") != nullptr ||
                                strstr(class_name, "Container") != nullptr ||
                                strstr(class_name, "Corpse") != nullptr;

                            if (!looks_like_container) continue;

                            std::string name = unity::get_name(obj);
                            if (name.empty())
                                name = class_name;

                            cached_containers.push_back({
                                obj, std::move(name)
                                });
                        }
                    }
                }

                const float container_max_dist = container_distance->get_value();
                const float container_max_dist_sq = container_max_dist * container_max_dist;

                new_loot.reserve(new_loot.size() + cached_containers.size());

                for (const auto& entry : cached_containers)
                {
                    Il2CppObject* cont_obj = entry.obj;
                    if (!cont_obj) continue;

                    Il2CppObject* game_object = unity::component_get_game_object(cont_obj);
                    if (!game_object) continue;

                    Il2CppObject* transform = unity::gameobject_get_transform(game_object);
                    if (!transform) continue;

                    unity::vector3 pos = unity::transform_get_pos(transform);

                    const float dx = my_pos.x - pos.x;
                    const float dy = my_pos.y - pos.y;
                    const float dz = my_pos.z - pos.z;
                    const float dist_sq = dx * dx + dy * dy + dz * dz;

                    if (dist_sq < min_dist_sq || dist_sq > container_max_dist_sq) continue;

                    new_loot.push_back({
                        entry.name, pos, true
                        });
                }
            }
            else
            {
                cached_containers.clear();
                cached_containers_last_refresh = 0;
            }

            loot_store.swap(new_loot);
        }
        else
        {
            loot_store.clear();
        }
    }

    void init() override
    {
    }

};