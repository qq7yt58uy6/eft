#pragma once
#include <fstream>

#include "../Module.h"
#include "../ModuleCategory.h"
#include "../../config/ActionRowValue.h"
#include "../../config/SelectValue.h"
#include "../../util/logger.h"

std::vector<Module*> static modules = {};

class ConfigModule : Module
{
public:
    ConfigModule(const std::vector<Module*>& m) : Module("配置", Config)
    {
        modules = m;
    }

    SelectValue* config = conf(new SelectValue(0, {
                                                   "Legit", "Rage", "Config 1", "Config 2", "Config 3", "Config 4",
                                                   "Config 5"
                                               }, "文件"));

    ActionRowValue* load = conf(new ActionRowValue([&]
    {
        load_config(config->get_selected());
    }, "加载"));

    ActionRowValue* save = conf(new ActionRowValue([&]
    {
        save_config(config->get_selected());
    }, "保存", true));

    void draw_overlay(ImDrawList* draw_list) override
    {
    }

    void application_update() override
    {
    }

    void gameworld_update(const Il2CppClass* game_world_class, Il2CppObjectInstance game_world_instance,
                          Il2CppObjectInstance main_player) override
    {
    }

    void init() override
    {
        load_config(config->get_selected());
    }

    static void load_config(const std::string& config)
    {
        if (globals::verbose) logger::info("Loading Config " + config);

        // try to create folder
        char* p_value;
        size_t len;
        errno_t _ = _dupenv_s(&p_value, &len, "APPDATA");
        std::string path = std::string(p_value) + "\\TarkovAddons";
        CreateDirectory(std::wstring(path.begin(), path.end()).c_str(), nullptr);

        // read file;
        std::ifstream t(path + "\\" + config);

        if (!t.good())
        {
            t = create_config(config);
        }

        std::stringstream buffer;
        buffer << t.rdbuf();
        t.close();

        // map keys to values
        auto res = utils::split(buffer.str(), ';');
        auto values = std::unordered_map<std::string, std::string>();
        for (size_t i = 0; i < res.size(); i += 2)
        {
            values[res.at(i)] = res.at(i + 1);
        }

        // write into modules
        for (auto module : modules)
        {
            module->apply([values](ConfigValue* c)
            {
                auto iter = values.find(c->get_imgui_title());
                if (iter == values.end()) c->set(c->get_default());
                else c->set(values.at(c->get_imgui_title()));
            });
        }
    }

    static void save_config(const std::string& config)
    {
        if (globals::verbose) logger::info("Saving Config " + config);

        // try to create folder
        char* p_value;
        size_t len;
        errno_t _ = _dupenv_s(&p_value, &len, "APPDATA");
        std::string path = std::string(p_value) + "\\TarkovAddons";
        CreateDirectory(std::wstring(path.begin(), path.end()).c_str(), nullptr);

        // create config string
        std::string value;
        for (auto module : modules)
        {
            module->apply([&value](ConfigValue* c)
            {
                value.append(c->get_imgui_title());
                value.append(";");
                value.append(c->get());
                value.append(";");
            });
        }

        // write file
        std::ofstream t(path + "\\" + config);
        t.write(value.c_str(), static_cast<std::streamsize>(value.size()));
        t.close();
    }

    static std::ifstream create_config(const std::string& config)
    {
        std::string value;
        for (auto module : modules)
        {
            module->apply([&value](ConfigValue* c)
            {
                value.append(c->get_imgui_title());
                value.append(";");
                value.append(c->get_default());
                value.append(";");
            });
        }

        // write file
        // try to create folder
        char* p_value;
        size_t len;
        errno_t _ = _dupenv_s(&p_value, &len, "APPDATA");
        std::string path = std::string(p_value) + "\\TarkovAddons";

        std::ofstream ot(path + "\\" + config);
        ot.write(value.c_str(), static_cast<std::streamsize>(value.size()));
        ot.close();

        return std::ifstream(path + "\\" + config);
    }
};
