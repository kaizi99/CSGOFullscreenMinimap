// Copyright (C) 2020  Kai-Uwe Zimdars

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "draw_config.h"

std::vector<draw_config> draw_config_parse_json(nlohmann::json input) {
    std::vector<draw_config> returnVector;

    for (auto config : input.items()) {
        draw_config extracted;

        extracted.name = config.key();
        extracted.drawTwoMaps = config.value()["drawTwoMaps"].get<bool>();
        extracted.drawName = config.value()["drawName"].get<bool>();
        extracted.nameCharacterSize = config.value()["nameCharacterSize"].get<int>();
        extracted.nameDeadCharacterSize = config.value()["nameDeadCharacterSize"].get<int>();
        extracted.circleSize = config.value()["circleSize"].get<float>();
        extracted.observerTextSize = config.value()["observerTextSize"].get<int>();
        extracted.bombIconScale = config.value()["bombIconScale"].get<float>();
        extracted.drawBauchbinde = config.value()["bauchbinde"].get<bool>();
        extracted.circleOutlineSize = config.value()["circleOutlineSize"].get<float>();

        returnVector.push_back(extracted);
    }

    return returnVector;
}

nlohmann::json draw_config_to_json(std::vector<draw_config> configs) {
    nlohmann::json returnJson;

    for (const draw_config& config : configs) {
        nlohmann::json configJson;

        configJson["drawTwoMaps"] = config.drawTwoMaps;
        configJson["drawName"] = config.drawName;
        configJson["nameCharacterSize"] = config.nameCharacterSize;
        configJson["nameDeadCharacterSize"] = config.nameDeadCharacterSize;
        configJson["circleSize"] = config.circleSize;
        configJson["observerTextSize"] = config.observerTextSize;
        configJson["bombIconScale"] = config.bombIconScale;
        configJson["bauchbinde"] = config.drawBauchbinde;
        configJson["circleOutlineSize"] = config.circleOutlineSize;

        returnJson[config.name] = configJson;
    }

    return returnJson;
}