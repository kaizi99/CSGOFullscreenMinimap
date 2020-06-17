//
// Created by Kai-Uwe Zimdars on 10.05.20.
//

#include "config_editor.h"

#include <imgui.h>

void config_editor::drawSettingsWindow() {
    if (m_drawWindow) {
        ImGui::Begin("Config Editor", &m_drawWindow);

        if (ImGui::TreeNode("Maps")) {
            ImGui::Columns(2);
            for (mapinfo& f : *m_mapConfigs) {
                ImGui::PushID(("map_ed_" + f.name).c_str());
                ImGui::Text("%s", f.name.c_str());
                ImGui::NextColumn();
                if (ImGui::Button("Edit")) {
                    m_editMap = &f;
                }
                ImGui::NextColumn();
                ImGui::PopID();
            }
            ImGui::Columns(1);
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

        if (m_editMap && ImGui::TreeNode("Edit Map")) {

            char* buffer = (char*)malloc(100);
            memset(buffer, 0, 100);

            strncpy(buffer, m_editMap->name.c_str(), 99);
            ImGui::InputText("Name", buffer, 100);
            m_editMap->name = buffer;

            strncpy(buffer, m_editMap->radarName.c_str(), 99);
            ImGui::InputText("Radar File", buffer, 100);
            m_editMap->radarName = buffer;

            ImGui::InputFloat3("Upper Left", &m_editMap->upperLeft.x, 1);
            ImGui::InputFloat("Scale", &m_editMap->scale, 1);
            ImGui::Checkbox("Two Layers", &m_editMap->hasTwoLayers);
            if (m_editMap->hasTwoLayers) {
                strncpy(buffer, m_editMap->lowerLayerName.c_str(), 99);
                ImGui::InputText("Lower Layer File", buffer, 100);
                m_editMap->lowerLayerName = buffer;

                ImGui::InputFloat("Cutoff", &m_editMap->cutoff);
            }

            free(buffer);

            ImGui::TreePop();
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

        ImGui::End();
    }
}

void config_editor::showWindow() {
    m_drawWindow = true;
}

