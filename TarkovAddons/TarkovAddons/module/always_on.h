#pragma once
#include "../il2cpp/Il2CppObjectInstance.h"
#include "../il2cpp/il2utils.h"

namespace always_on
{
    static inline Il2CppImage* assembly_csharp;

    using display_location_sig = bool(*)(Il2CppObject*, Il2CppObject*);

    static inline display_location_sig o_display_location = nullptr;

    inline bool hk_display_location(Il2CppObject* instance, Il2CppObject* location)
    {
        const bool res = o_display_location(instance, location);
        if (location != nullptr)
        {
            Il2CppObjectInstance location_instance(location);
            // force offline raid
            location_instance.set_field("ForceOnlineRaidInPVE", false);
            location_instance.set_field("ForceOfflineRaidInPVE", true);
            //location_instance.set_field("AccessKeys", nullptr);

            // No Terminal
            if (il2utils::conv_string(location_instance.get_field<Il2CppString*>("Name")) == "Terminal") return false;
            // Add Labyrinth
            if (il2utils::conv_string(location_instance.get_field<Il2CppString*>("Name")) == "Labyrinth") return true;

            if (il2utils::conv_string(location_instance.get_field<Il2CppString*>("Name")) == "Icebreaker") return true;
        }
        return res;
    }

    using transit_sig = Il2CppObject*(*)(Il2CppObject*, Il2CppObject*, int, Il2CppString*, Il2CppObject*,
                                         Il2CppObject*);
    static inline transit_sig o_transit = nullptr;

    inline Il2CppObject* hk_transit(Il2CppObject* instance, Il2CppObject* point, int player_count, Il2CppString* hash,
                                    Il2CppObject* keys, Il2CppObject* player)
    {
        // force offline transit
        return o_transit(instance, point, 1, il2cpp::il2cpp_string_new(""), keys, player);
    }

    inline void init()
    {
        // set assembly csharp image
        while (!il2utils::resolve_image("Assembly-CSharp.dll"))
        {
            Sleep(50);
        }
        assembly_csharp = il2utils::resolve_image("Assembly-CSharp.dll");

        // always offline raid and location unlock
        const Il2CppClass* match_maker_selection_location_screen = il2utils::resolve_class(
            assembly_csharp, "EFT.UI.Matchmaker", "MatchMakerSelectionLocationScreen");
        il2utils::hook_method(match_maker_selection_location_screen, "DisplayLocation", 1,
                              hk_display_location,
                              &o_display_location);

        // offline transits
        const Il2CppClass* transit_controller = il2utils::resolve_class(
            assembly_csharp, "EFT", "LocalTransitController");
        il2utils::hook_method(transit_controller, "Transit", 5,
                              hk_transit,
                              &o_transit);
    }
}
