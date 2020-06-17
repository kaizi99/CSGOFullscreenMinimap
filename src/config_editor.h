//
// Created by Kai-Uwe Zimdars on 10.05.20.
//

#pragma once

#include <vector>

#include "draw_config.h"
#include "map.h"

class config_editor {
public:
    config_editor(std::vector<mapinfo>* mapConfigs, std::vector<draw_config>* drawConfigs)
    : m_mapConfigs(mapConfigs), m_drawConfigs(drawConfigs), m_drawWindow(false)
    , m_editMap(nullptr), m_editConfig(nullptr) {}

    void drawSettingsWindow();
    void showWindow();
private:
    std::vector<mapinfo>* m_mapConfigs;
    std::vector<draw_config>* m_drawConfigs;

    mapinfo* m_editMap;
    draw_config* m_editConfig;

    bool m_drawWindow;
};
