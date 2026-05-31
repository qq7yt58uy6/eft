#pragma once
#include <chrono>
#include <random>

#include "../game_state.h"
#include "../Module.h"
#include "../ModuleCategory.h"
#include "../../config/IntTextboxValue.h"
#include "../../config/TextboxValue.h"
#include "../../il2cpp/Il2CppObjectInstance.h"


static Il2CppObject* item_factory;
using init_level_sig = Il2CppObject*(*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, bool, Il2CppObject*, Il2CppObject*,
                                        Il2CppObject*);
static init_level_sig o_init_level = nullptr;

inline Il2CppObject* hk_init_level(Il2CppObject* instance, Il2CppObject* item_factory_, Il2CppObject* config,
                                   const bool load_bundles_and_create_pools, Il2CppObject* resources,
                                   Il2CppObject* progress,
                                   Il2CppObject* ct)
{
    item_factory = item_factory_;
    return o_init_level(instance, item_factory_, config, load_bundles_and_create_pools, resources, progress, ct);
}

static bool no_limit = false;

using mod_can_be_moved_sig = bool*(*)(Il2CppObject*, Il2CppObject*);
static mod_can_be_moved_sig o_mod_can_be_moved = nullptr;

inline bool* hk_mod_can_be_moved(Il2CppObject* instance, Il2CppObject* container)
{
    bool* res = o_mod_can_be_moved(instance, container);
    if (no_limit) *res = true;
    return res;
}

using get_mod_raid_moddable_sig = bool(*)(Il2CppObject*);
static get_mod_raid_moddable_sig o_get_mod_raid_moddable = nullptr;

inline bool hk_get_mod_raid_moddable(Il2CppObject* instance)
{
    if (no_limit) return true;
    return o_get_mod_raid_moddable(instance);
}

// for some reason when using bool this was causing issues, probably missed something obvious
using slot_check_conditions_sig = PVOID(*)(PVOID, PVOID, PVOID, PVOID);

static slot_check_conditions_sig o_slot_check_conditions = nullptr;

inline PVOID hk_slot_check_conditions(PVOID instance, PVOID item, PVOID ignore_restrictions, PVOID ignore_malfunctions)
{
    if (no_limit)
        return
            o_slot_check_conditions(instance, item, reinterpret_cast<PVOID>(1), reinterpret_cast<PVOID>(1));
    return o_slot_check_conditions(instance, item, ignore_restrictions, ignore_malfunctions);
}

using is_mod_suitable_sig = bool(*)(Il2CppObject*, Il2CppObject*);

static is_mod_suitable_sig o_is_mod_suitable = nullptr;

inline bool hk_is_mod_suitable(Il2CppObject* instance, Il2CppObject* mod)
{
    if (no_limit) return true;
    return o_is_mod_suitable(instance, mod);
}

using is_ammo_compatible_sig = bool(*)(Il2CppObject*, Il2CppArray*);

static is_ammo_compatible_sig o_is_ammo_compatible = nullptr;

inline bool hk_is_ammo_compatible(Il2CppObject* instance, Il2CppArray* chambers)
{
    if (no_limit) return true;
    return o_is_ammo_compatible(instance, chambers);
}

using limited_at_address_sig = bool(*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, int*);

static limited_at_address_sig o_limited_at_address_inventory = nullptr;

inline bool hk_limited_at_address_inventory(Il2CppObject* instance, Il2CppObject* item, Il2CppObject* address,
                                            int* limit)
{
    if (no_limit) return false;
    return o_limited_at_address_inventory(instance, item, address, limit);
}

static limited_at_address_sig o_limited_at_address_item = nullptr;

inline bool hk_limited_at_address_item(Il2CppObject* instance, Il2CppObject* item, Il2CppObject* address,
                                       int* limit)
{
    if (no_limit) return false;
    return o_limited_at_address_item(instance, item, address, limit);
}

static limited_at_address_sig o_limited_at_address_backend = nullptr;

inline bool hk_limited_at_address_backend(Il2CppObject* instance, Il2CppObject* item, Il2CppObject* address,
                                          int* limit)
{
    if (no_limit) return false;
    return o_limited_at_address_backend(instance, item, address, limit);
}

using check_compat_sig = bool(*)(Il2CppObject*, Il2CppObject*);
static check_compat_sig o_check_compat_stack_slot = nullptr;

inline bool hk_check_compat_stack_slot(Il2CppObject* instance, Il2CppObject* item)
{
    if (no_limit) return true;
    return o_check_compat_stack_slot(instance, item);
}

static check_compat_sig o_check_compat_item_controller = nullptr;

inline bool hk_check_compat_item_controller(Il2CppObject* instance, Il2CppObject* item)
{
    if (no_limit) return true;
    return o_check_compat_item_controller(instance, item);
}

static check_compat_sig o_check_compat_magazine = nullptr;

inline bool hk_check_compat_magazine(Il2CppObject* instance, Il2CppObject* item)
{
    if (no_limit) return true;
    return o_check_compat_magazine(instance, item);
}

static check_compat_sig o_check_compat_virtual_container = nullptr;

inline bool hk_check_compat_virtual_container(Il2CppObject* instance, Il2CppObject* item)
{
    if (no_limit) return true;
    return o_check_compat_virtual_container(instance, item);
}

static check_compat_sig o_check_compat_slot = nullptr;

inline bool hk_check_compat_slot(Il2CppObject* instance, Il2CppObject* item)
{
    if (no_limit) return true;
    return o_check_compat_slot(instance, item);
}

static check_compat_sig o_check_compat_grid = nullptr;

inline bool hk_check_compat_grid(Il2CppObject* instance, Il2CppObject* item)
{
    if (no_limit) return true;
    return o_check_compat_grid(instance, item);
}

using check_restrictions_sig = bool(*)(Il2CppObject*, Il2CppObject*, Il2CppObject*);
static check_restrictions_sig o_check_restrictions = nullptr;

inline bool hk_check_restrictions(Il2CppObject* instance, Il2CppObject* item, Il2CppObject* controller)
{
    if (no_limit) return false;
    return o_check_restrictions(instance, item, controller);
}

static check_restrictions_sig o_check_special_slot = nullptr;

inline bool hk_check_special_slot(Il2CppObject* instance, Il2CppObject* item, Il2CppObject* controller)
{
    if (no_limit) return false;
    return o_check_special_slot(instance, item, controller);
}

static check_restrictions_sig o_check_slot_restrictions = nullptr;

inline bool hk_check_slot_restrictions(Il2CppObject* instance, Il2CppObject* item, Il2CppObject* controller)
{
    if (no_limit) return false;
    return o_check_slot_restrictions(instance, item, controller);
}

static bool big_grid = false;

using get_grid_size_sig = UINT64(*)(Il2CppObject*, Il2CppString*);
static get_grid_size_sig o_get_grid_size;

inline UINT64 hk_get_grid_size(Il2CppObject* instance, Il2CppString* profile_id)
{
    const UINT64 res = o_get_grid_size(instance, profile_id);

    if (big_grid)
    {
        if (globals::verbose) logger::info("Changing Transit Grid Size");
        // intvec2(0xA, 0xA)
        return 0xA0000000A;
    }

    return res;
}

using local_raid_ended_sig = Il2CppObject*(*)(Il2CppObject*, Il2CppObject*, Il2CppObject*, Il2CppArray*,
                                              unity::dict<Il2CppString*, Il2CppArray*>*);
static local_raid_ended_sig o_local_raid_ended;

inline Il2CppObject* hk_local_raid_ended(Il2CppObject* instance, Il2CppObject* settings, Il2CppObject* results,
                                         Il2CppArray* lost_insured_items,
                                         unity::dict<Il2CppString*, Il2CppArray*>* transfer_items)
{
    if (globals::verbose) logger::info("Local Raid Ended");
    if (globals::verbose)logger::info("Insured Items: " + std::to_string(lost_insured_items->max_length));
    if (globals::verbose)logger::info("Transfer Items: " + std::to_string(transfer_items->m_i_count));

    // Print out json for transited items
    /*
    for (int i = 0; i < transfer_items->m_i_count; ++i)
    {
        logger::info("  " + il2utils::conv_string(transfer_items->get_key_by_index(i)));
        logger::info("  " + std::to_string(transfer_items->get_value_by_index(i)->max_length));
        for (size_t j = 0; j < transfer_items->get_value_by_index(i)->max_length; ++j)
        {
            Il2CppObjectInstance flat_item(
                reinterpret_cast<Il2CppObject**>(&transfer_items->get_value_by_index(i)->data)[j]);
            const auto unparsed_data_ptr = flat_item.get_field<Il2CppObject*>("upd");
            if (!unparsed_data_ptr) continue;
            Il2CppObjectInstance unparsed_data(unparsed_data_ptr);
            const auto jtoken_ptr = unparsed_data.get_field<Il2CppObject*>("JToken");
            if (!jtoken_ptr) continue;
            Il2CppObjectInstance jtoken(jtoken_ptr);
            logger::info("      " + il2utils::conv_string(
                jtoken.get_method<Il2CppString*(*)(Il2CppObject*)>("ToString", 0)(jtoken.get_instance())));
        }
    }
    */

    return
        o_local_raid_ended(instance, settings, results, lost_insured_items, transfer_items);
}

class ItemModule : Module
{
public:
    ItemModule() : Module("物品生成", Exploit)
    {
    }

    TextboxValue* item_id = conf(new TextboxValue("", "物品 Id"));
    SelectValue* item_slot = conf(new SelectValue(4, {
                                                      "Primary Weapon", "Secondary Weapon", "Holster", "Melee",
                                                      "Backpack", "Secure Container", "Rig", "Armor", "Pockets",
                                                      "Eyewear", "FaceCover", "Headwear", "Earpiece", "Dog-tag",
                                                      "Armband"
                                                  }, "插槽"));
    CheckboxValue* override_max_amount = conf(new CheckboxValue(false, "无视最大堆叠数量"));
    IntTextboxValue* amount = conf(new IntTextboxValue(1, "物品数量"));
    CheckboxValue* fir = conf(new CheckboxValue(true, "战局内拾取"));
    CheckboxValue* replace_slot = conf(new CheckboxValue(false, "替换当前槽位"));

    ActionRowValue* create_item = conf(new ActionRowValue([&]
        {
            create_item_impl();
        }, "生成物品")
    );
    ActionRowValue* create_quest_item = conf(new ActionRowValue([&]
        {
            create_quest_item_impl();
        }, "生成任务物品", true)
    );
    ActionRowValue* transit_item = conf(new ActionRowValue([&]
        {
            transit_item_impl();
        }, "转移物品 (2-3小时)", true)
    );

    CheckboxValue* no_slot_limits = conf(
        new CheckboxValue(false, "无视槽位限制"));

    CheckboxValue* increased_transfer_size = conf(
        new CheckboxValue(false, "扩大转移容量"));

    bool queue_create_item = false;
    bool queue_create_quest_item = false;
    bool queue_transit_item = false;

    void create_item_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_create_item = true;
    }

    void create_quest_item_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_create_quest_item = true;
    }

    void transit_item_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_transit_item = true;
    }

    void draw_overlay(ImDrawList* draw_list) override
    {
    }

    void application_update() override
    {
        no_limit = no_slot_limits->get_value();
        big_grid = increased_transfer_size->get_value();
    }

    static void exec_transit_item(Il2CppObjectInstance game_world_instance, Il2CppObjectInstance main_player,
                                  Il2CppObject* item)
    {
        Il2CppObjectInstance transit_controller(
            game_world_instance.get_field<Il2CppObject*>("<TransitController>k__BackingField"));

        Il2CppObjectInstance transfer_items_controller(
            transit_controller.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_TransferItemsController", 0)(
                transit_controller.get_instance()));

        Il2CppString* player_id = main_player.get_method<Il2CppString*(*)(Il2CppObject*)>("get_ProfileId", 0)(
            main_player.get_instance());
        Il2CppObject* transit_stash_ptr = transfer_items_controller.get_method<Il2CppObject*(
            *)(Il2CppObject*, Il2CppString*)>("GetOrAddTransferContainer", 1)(
            transfer_items_controller.get_instance(), player_id);

        transfer_items_controller.get_method<void(
            *)(Il2CppObject*, Il2CppObject*)>("InitPlayerStash", 1)(
            transfer_items_controller.get_instance(), main_player.get_instance());

        Il2CppObjectInstance transfer_stash(transit_stash_ptr);

        Il2CppObjectInstance transfer_grid(
            transfer_stash.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Grid", 0)(
                transfer_stash.get_instance()));

        Il2CppObject* space = transfer_grid.get_method<Il2CppObject*(
            *)(Il2CppObject*, Il2CppObject*)>("FindFreeSpace", 1)(transfer_grid.get_instance(), item);

        if (space)
        {
            transfer_grid.get_method<void(*)(Il2CppObject*, Il2CppObject*, Il2CppObject*)>("PlaceItem", 2)(
                transfer_grid.get_instance(), item, space);

            if (globals::verbose) logger::info("Moved Transit Item");
        }

        transfer_items_controller.get_method<void(
            *)(Il2CppObject*, Il2CppObject*)>("MoveItemsFromTempStashToTransferStash", 1)(
            transfer_items_controller.get_instance(), main_player.get_instance());
    }

    void gameworld_update(const Il2CppClass* game_world_class, Il2CppObjectInstance game_world_instance,
                          Il2CppObjectInstance main_player) override
    {
        if (queue_create_item || queue_create_quest_item || queue_transit_item)
        {
            if (!item_factory)
            {
                if (globals::verbose) logger::error("No Item Factory");
                queue_create_item = false;
                queue_create_quest_item = false;
                queue_transit_item = false;
                return;
            }

            Il2CppObjectInstance item_factory_instance(item_factory);
            const auto create_item_method = item_factory_instance.get_method<Il2CppObject*(*)(
                Il2CppObject*, Il2CppString*)>("GetPresetItem", 1);

            Il2CppObject* item = create_item_method(item_factory,
                                                    il2cpp::il2cpp_string_new(item_id->get().c_str()));

            if (globals::verbose) logger::info("Created Item: " + item_id->get());

            Il2CppObjectInstance inventory_controller(
                main_player.get_field<Il2CppObject*>("_inventoryController"));

            Il2CppObjectInstance inventory(
                inventory_controller.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Inventory", 0)(
                    inventory_controller.get_instance()));

            // create into inventory
            if (queue_create_item)
            {
                queue_create_item = false;

                Il2CppObjectInstance equipment(
                    inventory.get_field<Il2CppObject*>("Equipment"));

                Il2CppObjectInstance equipment_slot(
                    equipment.get_method<Il2CppObject*(*)(Il2CppObject*, int)>("GetSlot", 1)(
                        equipment.get_instance(), item_slot->get_value()));

                if (!replace_slot->get_value())
                {
                    Il2CppObject* item_ptr = equipment_slot.get_method<Il2CppObject*(*)(Il2CppObject*)>(
                        "get_ContainedItem", 0)(
                        equipment_slot.get_instance());

                    if (item_ptr)
                    {
                        Il2CppObjectInstance slot_item_instance(item_ptr);
                        const auto grids = slot_item_instance.get_field<Il2CppArray*>("Grids");
                        for (size_t i = 0; i < grids->max_length; ++i)
                        {
                            Il2CppObjectInstance grid(reinterpret_cast<Il2CppObject**>(&grids->data)[0]);
                            Il2CppObject* space = grid.get_method<Il2CppObject*(
                                *)(Il2CppObject*, Il2CppObject*)>("FindFreeSpace", 1)(grid.get_instance(), item);

                            if (!space) continue;

                            grid.get_method<void(*)(Il2CppObject*, Il2CppObject*, Il2CppObject*)>("PlaceItem", 2)(
                                grid.get_instance(), item, space);

                            if (globals::verbose) logger::info("Placed Item");

                            break;
                        }
                    }
                }
                else
                {
                    equipment_slot.get_method<Il2CppObject*(*)(Il2CppObject*, Il2CppObject*)>("Add", 1)(
                        equipment_slot.get_instance(), item);
                }
            }
            // create into quest inventory
            else if (queue_create_quest_item)
            {
                queue_create_quest_item = false;

                Il2CppObjectInstance quest_raid_items(inventory.get_field<Il2CppObject*>("QuestRaidItems"));
                Il2CppObjectInstance stash_grid(
                    quest_raid_items.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Grid", 0)(
                        quest_raid_items.get_instance()));

                Il2CppObject* space = stash_grid.get_method<Il2CppObject*(
                    *)(Il2CppObject*, Il2CppObject*)>("FindFreeSpace", 1)(stash_grid.get_instance(), item);

                if (space)
                {
                    stash_grid.get_method<void(*)(Il2CppObject*, Il2CppObject*, Il2CppObject*)>("PlaceItem", 2)(
                        stash_grid.get_instance(), item, space);
                    if (globals::verbose) logger::info("Placed Quest Item");
                }
            }
            // create into transit inventory
            else if (queue_transit_item)
            {
                queue_transit_item = false;
                exec_transit_item(game_world_instance, main_player, item);
            }

            Il2CppObjectInstance item_instance(item);

            if (override_max_amount->get_value())
            {
                item_instance.set_field("StackObjectsCount", amount->get_value());
            }
            else
            {
                int max_size = item_instance.get_method<int(*)(Il2CppObject*)>("get_StackMaxSize", 0)(
                    item_instance.get_instance());
                item_instance.set_field("StackObjectsCount", min(amount->get_value(), max_size));
            }

            if (fir->get_value())
            {
                item_instance.get_method<void(*)(Il2CppObject*, bool)>("set_SpawnedInSession", 1)(item, true);
            }
        }
    }

    void init() override
    {
        const Il2CppImage* assembly = il2utils::resolve_image("Assembly-CSharp.dll");

        // item factory
        const Il2CppClass* game_world_class = il2utils::resolve_class(assembly, "EFT", "GameWorld");
        il2utils::hook_method(game_world_class, "InitLevel", 6, hk_init_level, &o_init_level);

        // slot limitss
        const Il2CppClass* item_controller_class = il2utils::resolve_class(
            assembly, "EFT.InventoryLogic", "ItemController");
        const Il2CppClass* inventory_controller_class = il2utils::resolve_class(
            assembly, "EFT.InventoryLogic", "InventoryController");
        const Il2CppClass* backend_inventory_controller_class = il2utils::resolve_class(
            assembly, "EFT.InventoryLogic", "BackEndInventoryController");

        il2utils::hook_method(item_controller_class, "IsLimitedAtAddress", 3, hk_limited_at_address_item,
                              &o_limited_at_address_item);
        il2utils::hook_method(inventory_controller_class, "IsLimitedAtAddress", 3, hk_limited_at_address_inventory,
                              &o_limited_at_address_inventory);
        il2utils::hook_method(backend_inventory_controller_class, "IsLimitedAtAddress", 3,
                              hk_limited_at_address_backend,
                              &o_limited_at_address_backend);


        const Il2CppClass* stack_slot_class = il2utils::resolve_class(assembly, "EFT.InventoryLogic", "StackSlot");
        il2utils::hook_method(stack_slot_class, "CheckCompatibility", 1, hk_check_compat_stack_slot,
                              &o_check_compat_stack_slot);

        il2utils::hook_method(item_controller_class, "CheckCompatibility", 1, hk_check_compat_item_controller,
                              &o_check_compat_item_controller);
        il2utils::hook_method(item_controller_class, "CheckRestrictions", 2, hk_check_restrictions,
                              &o_check_restrictions);
        il2utils::hook_method(item_controller_class, "CheckForOnlySpecialSlotsItem", 2, hk_check_special_slot,
                              &o_check_special_slot);
        il2utils::hook_method(item_controller_class, "CheckSlotRestrictions", 2, hk_check_slot_restrictions,
                              &o_check_slot_restrictions);

        const Il2CppClass* magazine_class = il2utils::resolve_class(assembly, "EFT.InventoryLogic", "Magazine");
        il2utils::hook_method(magazine_class, "CheckCompatibility", 1, hk_check_compat_magazine,
                              &o_check_compat_magazine);
        il2utils::hook_method(magazine_class, "IsAmmoCompatible", 1, hk_is_ammo_compatible,
                              &o_is_ammo_compatible);

        const Il2CppClass* virtual_container_class = il2utils::resolve_class(
            assembly, "EFT.InventoryLogic", "VirtualContainer");
        il2utils::hook_method(virtual_container_class, "CheckCompatibility", 1, hk_check_compat_virtual_container,
                              &o_check_compat_virtual_container);

        const Il2CppClass* grid_class = il2utils::resolve_class(assembly, "EFT.InventoryLogic", "Grid");
        il2utils::hook_method(grid_class, "CheckCompatibility", 1, hk_check_compat_grid, &o_check_compat_grid);

        const Il2CppClass* slot_class = il2utils::resolve_class(assembly, "EFT.InventoryLogic", "Slot");
        il2utils::hook_method(slot_class, "CheckCompatibility", 1, hk_check_compat_slot, &o_check_compat_slot);
        il2utils::hook_method(slot_class, "CheckConditions", 3, hk_slot_check_conditions, &o_slot_check_conditions);

        const Il2CppClass* weapon_class = il2utils::resolve_class(assembly, "EFT.InventoryLogic", "Weapon");
        il2utils::hook_method(weapon_class, "IsModSuitable", 1, hk_is_mod_suitable, &o_is_mod_suitable);

        const Il2CppClass* mod_class = il2utils::resolve_class(assembly, "EFT.InventoryLogic", "Mod");
        il2utils::hook_method(mod_class, "get_RaidModdable", 0, hk_get_mod_raid_moddable, &o_get_mod_raid_moddable);
        il2utils::hook_method(mod_class, "CanBeMoved", 1, hk_mod_can_be_moved, &o_mod_can_be_moved);

        // grid size
        const Il2CppClass* transfer_items_controller = il2utils::resolve_class(
            assembly, "EFT", "TransferItemsController");
        il2utils::hook_method(transfer_items_controller, "GetGridSizeForProfileId", 1, hk_get_grid_size,
                              &o_get_grid_size);

        // local raid ended
        const Il2CppClass* eft_client_backend_session = il2utils::resolve_class(
            assembly, "EFT", "EftClientBackendSession");
        il2utils::hook_method(eft_client_backend_session, "LocalRaidEnded", 4, hk_local_raid_ended,
                              &o_local_raid_ended);
    }
};
