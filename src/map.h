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

#pragma once

#include <SFML/Graphics.hpp>

#include "draw_config.h"

struct view {
    float centerX;
    float centerY;
    float width;
    float height;

    sf::View getSFMLView();
};

struct mapinfo {
    std::string name;
    std::string radarName;
    sf::Vector3f upperLeft;
    float scale;
    bool hasTwoLayers;
    std::string lowerLayerName;
    float cutoff;
    sf::Vector2f lowerLayerOffset;
    view standardView;
    view aSiteView;
    view bSiteView;
};

std::vector<mapinfo> mapinfo_parse_json(nlohmann::json input);
nlohmann::json mapinfo_to_json(std::vector<mapinfo> input);

nlohmann::json encodeView(const view& v);

struct loadedMap {
    mapinfo map;
    sf::Texture mapTexture;
    sf::Texture mapTextureLower;
    sf::Sprite mapSprite;
    sf::Sprite mapSpriteLower;
};

loadedMap* loadMap(std::string map, const std::vector<mapinfo>& maps, sf::RenderWindow& window, draw_config config);