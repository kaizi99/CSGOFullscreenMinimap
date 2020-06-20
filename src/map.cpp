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

view parseView(const nlohmann::json& input) {
    view v;

    v.centerX = input["centerX"].get<float>();
    v.centerY = input["centerY"].get<float>();
    v.width = input["width"].get<float>();
    v.height = input["height"].get<float>();

    return v;
}

nlohmann::json encodeView(const view& v) {
    nlohmann::json j;

    j["centerX"] = v.centerX;
    j["centerY"] = v.centerY;
    j["width"] = v.width;
    j["height"] = v.height;

    return j;
}

sf::View view::getSFMLView() {
    return sf::View(sf::Vector2f(centerX, centerY), sf::Vector2f(width, height));
}

std::vector<mapinfo> mapinfo_parse_json(nlohmann::json input) {
    std::vector<mapinfo> returnVector;

    for (const auto& map : input.items()) {
        mapinfo info;

        info.name = map.key();
        info.radarName = map.value()["radarName"].get<std::string>();
        info.upperLeft.x = map.value()["upperLeft"]["x"].get<float>();
        info.upperLeft.y = map.value()["upperLeft"]["y"].get<float>();
        info.scale = map.value()["scale"].get<float>();

        info.standardView = parseView(map.value()["standardView"]);
        info.aSiteView = parseView(map.value()["aSiteView"]);
        info.bSiteView = parseView(map.value()["bSiteView"]);

        if (!map.value()["lowerLayerName"].is_null()) {
            info.hasTwoLayers = true;
            info.lowerLayerName = map.value()["lowerLayerName"].get<std::string>();
            info.cutoff = map.value()["cutoff"].get<float>();
            info.lowerLayerOffset.x = map.value()["lowerOffset"]["x"].get<float>();
            info.lowerLayerOffset.y = map.value()["lowerOffset"]["y"].get<float>();
        } else {
            info.hasTwoLayers = false;
            info.cutoff = 0.0f;
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
            mapJson["lowerOffset"]["x"] = info.lowerLayerOffset.x;
            mapJson["lowerOffset"]["y"] = info.lowerLayerOffset.y;
        }

        mapJson["standardView"]["centerX"] = info.standardView.centerX;
        mapJson["standardView"]["centerY"] = info.standardView.centerY;
        mapJson["standardView"]["width"] = info.standardView.width;
        mapJson["standardView"]["height"] = info.standardView.height;

        mapJson["aSiteView"]["centerX"] = info.aSiteView.centerX;
        mapJson["aSiteView"]["centerY"] = info.aSiteView.centerY;
        mapJson["aSiteView"]["width"] = info.aSiteView.width;
        mapJson["aSiteView"]["height"] = info.aSiteView.height;

        mapJson["bSiteView"]["centerX"] = info.bSiteView.centerX;
        mapJson["bSiteView"]["centerY"] = info.bSiteView.centerY;
        mapJson["bSiteView"]["width"] = info.bSiteView.width;
        mapJson["bSiteView"]["height"] = info.bSiteView.height;

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
            returnMap->mapSpriteLower.setPosition(selectedMap.lowerLayerOffset);
        }
    }

    return returnMap;
}