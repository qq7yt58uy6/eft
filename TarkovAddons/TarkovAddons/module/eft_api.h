#pragma once
#include "../il2cpp/unity.h"

namespace eft_api
{
    inline unity::vector3 world_to_screen(Il2CppObject* camera, unity::vector3 world_point, float zoom)
    {
        unity::vector3 screen_point = unity::world_to_screen_point(camera, world_point);
        const float scale = menu::get_height() / static_cast<float>(unity::camera_pixel_height(camera));
        screen_point.y = menu::get_height() - screen_point.y * scale;
        screen_point.x *= scale;

        if (zoom > 1)
        {
            screen_point.x -= menu::get_width() / 2;
            screen_point.y -= menu::get_height() / 2;
            screen_point.x *= zoom;
            screen_point.y *= zoom;
            screen_point.x += menu::get_width() / 2;
            screen_point.y += menu::get_height() / 2;
        }

        return screen_point;
    }

    inline float get_scope_zoom(Il2CppObjectInstance& player)
    {
        Il2CppObject* hands_controller_object = player.get_method<Il2CppObject*(
            *)(Il2CppObject*)>("get_HandsController", 0)(player.get_instance());

        if (hands_controller_object && std::string(hands_controller_object->klass->name) == "FirearmController")
        {
            Il2CppObjectInstance firearm_controller(hands_controller_object);

            if (!firearm_controller.get_method<bool(*)(Il2CppObject*)>("get_IsAiming", 0)(hands_controller_object))
                return 1;

            Il2CppObjectInstance procedural_weapon_animation(
                player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_ProceduralWeaponAnimation", 0)(
                    player.get_instance()));

            const auto mod_ptr = procedural_weapon_animation.get_method<Il2CppObject*(*)(Il2CppObject*)>(
                "get_CurrentAimingMod", 0)(
                procedural_weapon_animation.get_instance());

            if (!mod_ptr) return 1;

            Il2CppObjectInstance sight_component(mod_ptr);
            const float zoom = sight_component.get_field<float>("ScopeZoomValue");
            auto adjustable_data = Il2CppObjectInstance(
                sight_component.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_AdjustableOpticData", 0)(
                    sight_component.get_instance()));

            // not a magnifying scope
            if (zoom == 0.0f) return 1;

            const auto minmax = adjustable_data.get_field<unity::vector3>("MinMaxFov");

            // calculate the actual zoom by dividing by max fov;
            return minmax.x / zoom;
        }

        return 1;
    }

    inline bool is_visible(const unity::vector3& screen_point)
    {
        return screen_point.z > 0.01f && screen_point.x > -100.0f && screen_point.y > -100.0f && screen_point.x <
            menu::get_width() + 100.0f && screen_point.y < menu::get_height() + 100.0f;
    }

    static std::unordered_map<wchar_t, std::string> converted_letters =
    {
        {L'а', "a"},
        {L'б', "b"},
        {L'в', "v"},
        {L'г', "g"},
        {L'д', "d"},
        {L'е', "e"},
        {L'ё', "yo"},
        {L'ж', "zh"},
        {L'з', "z"},
        {L'и', "i"},
        {L'й', "j"},
        {L'к', "k"},
        {L'л', "l"},
        {L'м', "m"},
        {L'н', "n"},
        {L'о', "o"},
        {L'п', "p"},
        {L'р', "r"},
        {L'с', "s"},
        {L'т', "t"},
        {L'у', "u"},
        {L'ф', "f"},
        {L'х', "h"},
        {L'ц', "c"},
        {L'ч', "ch"},
        {L'ш', "sh"},
        {L'щ', "sch"},
        {L'ъ', "j"},
        {L'ы', "i"},
        {L'ь', "j"},
        {L'э', "e"},
        {L'ю', "yu"},
        {L'я', "ya"},
        {L'А', "A"},
        {L'Б', "B"},
        {L'В', "V"},
        {L'Г', "G"},
        {L'Д', "D"},
        {L'Е', "E"},
        {L'Ё', "Yo"},
        {L'Ж', "Zh"},
        {L'З', "Z"},
        {L'И', "I"},
        {L'Й', "J"},
        {L'К', "K"},
        {L'Л', "L"},
        {L'М', "M"},
        {L'Н', "N"},
        {L'О', "O"},
        {L'П', "P"},
        {L'Р', "R"},
        {L'С', "S"},
        {L'Т', "T"},
        {L'У', "U"},
        {L'Ф', "F"},
        {L'Х', "H"},
        {L'Ц', "C"},
        {L'Ч', "Ch"},
        {L'Ш', "Sh"},
        {L'Щ', "Sch"},
        {L'Ъ', "J"},
        {L'Ы', "I"},
        {L'Ь', "J"},
        {L'Э', "E"},
        {L'Ю', "Yu"},
        {L'Я', "Ya"}
    };

    inline std::string to_english(const std::wstring& input)
    {
        std::stringstream out;
        for (wchar_t c : input)
        {
            if (converted_letters.contains(c))
            {
                out << converted_letters.at(c);
            }
            else
            {
                out << static_cast<char>(c);
            }
        }
        return out.str();
    }
}
