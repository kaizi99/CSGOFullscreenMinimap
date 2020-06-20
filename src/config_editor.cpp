//
// Created by Kai-Uwe Zimdars on 10.05.20.
//

#include "config_editor.h"

#include <imgui.h>
#include <fstream>

void config_editor::drawSettingsWindow(const sf::RenderWindow& window) {
    if (m_drawWindow) {
        ImGui::Begin("Config Editor", &m_drawWindow);

        if (ImGui::TreeNode("Maps")) {
            for (mapinfo& f : *m_mapConfigs) {
                ImGui::PushID(("map_ed_" + f.name).c_str());

                if (ImGui::TreeNode(f.name.c_str())) {
                    char* buffer = (char*)malloc(100);
                    memset(buffer, 0, 100);

                    strncpy(buffer, f.name.c_str(), 99);
                    ImGui::InputText("Name", buffer, 100);
                    f.name = buffer;

                    strncpy(buffer, f.radarName.c_str(), 99);
                    ImGui::InputText("Radar File", buffer, 100);
                    f.radarName = buffer;

                    ImGui::InputFloat3("Upper Left", &f.upperLeft.x);
                    ImGui::InputFloat("Scale", &f.scale, 1);
                    ImGui::Checkbox("Two Layers", &f.hasTwoLayers);
                    if (f.hasTwoLayers) {
                        strncpy(buffer, f.lowerLayerName.c_str(), 99);
                        ImGui::InputText("Lower Layer File", buffer, 100);
                        f.lowerLayerName = buffer;

                        ImGui::InputFloat("Cutoff", &f.cutoff);

                        ImGui::InputFloat2("Map Offset", &f.lowerLayerOffset.x);
                    }

                    if (ImGui::Button("Set as default view")) {
                        sf::Vector2f center = window.getView().getCenter();
                        sf::Vector2f size = window.getView().getSize();

                        f.standardView.centerX = center.x;
                        f.standardView.centerY = center.y;
                        f.standardView.width = size.x;
                        f.standardView.height = size.y;
                    }

                    if (ImGui::Button("Set as A Site view")) {
                        sf::Vector2f center = window.getView().getCenter();
                        sf::Vector2f size = window.getView().getSize();

                        f.aSiteView.centerX = center.x;
                        f.aSiteView.centerY = center.y;
                        f.aSiteView.width = size.x;
                        f.aSiteView.height = size.y;
                    }

                    if (ImGui::Button("Set as B Site view")) {
                        sf::Vector2f center = window.getView().getCenter();
                        sf::Vector2f size = window.getView().getSize();

                        f.bSiteView.centerX = center.x;
                        f.bSiteView.centerY = center.y;
                        f.bSiteView.width = size.x;
                        f.bSiteView.height = size.y;
                    }

                    free(buffer);

                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Draw Configs")) {
            ImGui::Columns(2);
            for (draw_config& c : *m_drawConfigs) {
                ImGui::PushID(("conf_ed_" + c.name).c_str());
                ImGui::Text("%s", c.name.c_str());
                ImGui::NextColumn();
                if (ImGui::Button("Edit")) {
                    m_editConfig = &c;
                }
                ImGui::NextColumn();
                ImGui::PopID();
            }
            ImGui::TreePop();
            ImGui::Columns(1);
        }

        if (m_editConfig && ImGui::TreeNode("Edit Draw Config")) {
            char* buffer = (char*)malloc(100);
            memset(buffer, 0, 100);

            strncpy(buffer, m_editConfig->name.c_str(), 99);
            ImGui::InputText("Name", buffer, 100);
            m_editConfig->name = buffer;

            ImGui::Checkbox("Draw Two Maps", &m_editConfig->drawTwoMaps);
            ImGui::Checkbox("Draw Name", &m_editConfig->drawName);
            ImGui::InputInt("Name Size", &m_editConfig->nameCharacterSize);
            ImGui::InputInt("Name Size (dead)", &m_editConfig->nameDeadCharacterSize);
            ImGui::InputFloat("Circle Size", &m_editConfig->circleSize, 1);
            ImGui::InputInt("Observerslot Text Size", &m_editConfig->observerTextSize);
            ImGui::InputFloat("Bomb Scale", &m_editConfig->bombIconScale);

            ImGui::TreePop();
        }

        ImGui::InputInt("Gamestate Integration Port", &m_gamestatePort, 1, 100);

        if (ImGui::Button("Save config")) {
            std::ofstream fs("config.json", std::ios::trunc);

            nlohmann::json configJson;
            configJson["maps"] = mapinfo_to_json(*m_mapConfigs);
            configJson["configs"] = draw_config_to_json(*m_drawConfigs);
            configJson["gamestatePort"] = m_gamestatePort;

            fs << configJson.dump(4);

            fs.close();
        }

        ImGui::End();
    }
}

void config_editor::showWindow() {
    m_drawWindow = true;
}

