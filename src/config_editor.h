//
// Created by Kai-Uwe Zimdars on 10.05.20.
//

#pragma once

#include <vector>

#include "draw_config.h"
#include "map.h"

class config_editor {
public:
    config_editor(std::vector<mapinfo>* mapConfigs, std::vector<draw_config>* drawConfigs, int gamestatePort)
    : m_mapConfigs(mapConfigs), m_drawConfigs(drawConfigs), m_drawWindow(false)
     ,m_editConfig(nullptr), m_gamestatePort(gamestatePort) {}

    void drawSettingsWindow(const sf::RenderWindow& window);
    void showWindow();
private:
    std::vector<mapinfo>* m_mapConfigs;
    std::vector<draw_config>* m_drawConfigs;

    draw_config* m_editConfig;
    int m_gamestatePort;

    bool m_drawWindow;
};
