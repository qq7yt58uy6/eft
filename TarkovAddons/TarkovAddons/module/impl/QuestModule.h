#pragma once
#include "../game_state.h"
#include "../Module.h"
#include "../ModuleCategory.h"
#include "../../il2cpp/unity.h"

class QuestModule : Module
{
public:
    ~QuestModule() override
    {
        for (auto option : quest_select->get_options())
        {
            delete[] option;
        }
    }

    QuestModule() : Module("任务", Exploit)
    {
    }

    SelectValue* quest_select = conf(new SelectValue(0, {}, "任务"));
    ActionRowValue* complete = conf(new ActionRowValue([&]
    {
        complete_impl();
    }, "完成任务"));
    ActionRowValue* complete_all = conf(new ActionRowValue([&]
    {
        complete_all_impl();
    }, "完成全部任务", true));

    bool queue_unlock = false;
    bool queue_unlock_all = false;

    void complete_all_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_unlock_all = true;
    }

    void complete_impl()
    {
        if (!game_state::is_in_raid) return;
        queue_unlock = true;
    }

    bool names_lock = false;
    std::vector<Il2CppString*> quest_names = {};

    void draw_overlay(ImDrawList* draw_list) override
    {
        if (names_lock) return;
        names_lock = true;
        if (static_cast<size_t>(quest_select->get_value()) > quest_names.size())
        {
            quest_select->set("0");
        }
        for (auto option : quest_select->get_options())
        {
            delete[] option;
        }
        std::vector<const char*> new_quest_names = {};
        for (auto quest_name : quest_names)
        {
            std::string new_name = il2utils::conv_string(quest_name);
            const auto allocated_string = new char[new_name.size() + 1];
            memcpy(allocated_string, new_name.c_str(), new_name.size());
            allocated_string[new_name.size()] = NULL;
            new_quest_names.push_back(allocated_string);
        }
        quest_select->set_options(new_quest_names);
        names_lock = false;
    }

    void application_update() override
    {
    }

    void gameworld_update(const Il2CppClass* game_world_class, Il2CppObjectInstance game_world_instance,
                          Il2CppObjectInstance main_player) override
    {
        if (names_lock) return;
        names_lock = true;
        Il2CppObjectInstance quest_controller(
            main_player.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_QuestController", 0)(
                main_player.get_instance()));

        Il2CppObjectInstance quest_book(
            quest_controller.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_Quests", 0)(
                quest_controller.get_instance()));

        const auto status_data = quest_book.get_field<unity::list*>("_questsData");

        quest_names.clear();

        for (size_t i = 0; i < status_data->m_p_list_array->max_length; ++i)
        {
            Il2CppObject* quest_data_ptr = reinterpret_cast<Il2CppObject**>(&status_data->m_p_list_array->data)[i];
            if (!quest_data_ptr) continue;

            Il2CppObjectInstance quest_data(quest_data_ptr);

            const auto quest_ptr = quest_data.get_field<Il2CppObject*>("Quest");
            if (!quest_ptr) continue;

            Il2CppObjectInstance quest(quest_ptr);

            Il2CppObjectInstance quest_status(quest.get_field<Il2CppObject*>("_statusData"));

            // 2 = Started
            if (quest_status.get_field<int>("Status") != 2) continue;

            Il2CppString* quest_name =
                quest.get_method<Il2CppString*(*)(Il2CppObject*)>("get_Name", 0)(quest.get_instance());

            Il2CppObjectInstance quest_template(quest.get_field<Il2CppObject*>("_template"));
            Il2CppString* quest_id = quest_template.get_method<Il2CppString*(*)(Il2CppObject*)>("get_Id", 0)(
                quest_template.get_instance());

            quest_names.push_back(quest_name);

            if (queue_unlock_all || (queue_unlock && il2utils::conv_string(quest_name) == quest_select->get_selected()))
            {
                if (globals::verbose) logger::info("Unlocking Quest: " + il2utils::conv_string(quest_name));

                quest_controller.get_method<Il2CppObject*(*)(Il2CppObject*, Il2CppString*)>(
                    "SendQuestCompleteToBackend", 1)(
                    quest_controller.get_instance(), quest_id);
            }
        }

        if (queue_unlock_all)
        {
            queue_unlock_all = false;
        }
        if (queue_unlock)
        {
            queue_unlock = false;
        }
        names_lock = false;
    }

    void init() override
    {
    }
};
