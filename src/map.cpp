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

#include "map.h"

std::vector<mapinfo> mapinfo_parse_json(nlohmann::json input) {
    std::vector<mapinfo> returnVector;

    for (const auto& map : input.items()) {
        mapinfo info;

        info.name = map.key();
        info.radarName = map.value()["radarName"].get<std::string>();
        info.upperLeft.x = map.value()["upperLeft"]["x"].get<int>();
        info.upperLeft.y = map.value()["upperLeft"]["y"].get<int>();
        info.scale = map.value()["scale"].get<float>();

        if (!map.value()["lowerLayerName"].is_null()) {
            info.hasTwoLayers = true;
            info.lowerLayerName = map.value()["lowerLayerName"].get<std::string>();
            info.cutoff = map.value()["cutoff"].get<int>();
        }

        returnVector.push_back(info);
    }

    return returnVector;
}

nlohmann::json mapinfo_to_json(std::vector<mapinfo> input) {
    nlohmann::json returnJson;

    for (const mapinfo& info : input) {
        nlohmann::json mapJson;
        
        mapJson["name"] = info.name;
        mapJson["radarName"] = info.radarName;
        mapJson["upperLeft"]["x"] = info.upperLeft.x;
        mapJson["upperLeft"]["y"] = info.upperLeft.y;
        mapJson["scale"] = info.scale;

        if (info.hasTwoLayers) {
            mapJson["lowerLayerName"] = info.lowerLayerName;
            mapJson["cutoff"] = info.cutoff;
        }

        returnJson[info.name] = mapJson;
    }

    return returnJson;
}

loadedMap* loadMap(std::string map, const std::vector<mapinfo>& maps, sf::RenderWindow& window, draw_config config) {
    // Search for map in array (yes I know there is a binary search but the dataset is really
    // small and unsorted so this is okay to do)
    mapinfo selectedMap;

    bool mapFound = false;

    for (const mapinfo& iMap : maps) {
        if (iMap.name == map) {
            selectedMap = iMap;
            mapFound = true;
        }
    }

    if (!mapFound) {
        return nullptr;
    }

    // allocate new map info
    loadedMap* returnMap = new loadedMap();

    // load map info
    returnMap->map = selectedMap;
    returnMap->mapTexture.loadFromFile(selectedMap.radarName);
    returnMap->mapSprite = sf::Sprite(returnMap->mapTexture);

    if (selectedMap.hasTwoLayers) {
        returnMap->mapTextureLower.loadFromFile(selectedMap.lowerLayerName);
        returnMap->mapSpriteLower = sf::Sprite(returnMap->mapTextureLower);
        
        if (config.drawTwoMaps) {
            returnMap->mapSpriteLower.setPosition(sf::Vector2f(1024, 0));
        }
    }

    return returnMap;
}