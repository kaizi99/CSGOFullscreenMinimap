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

loadedMap* loadMap(std::string map, mapinfo* maps, int mapCount, sf::RenderWindow& window) {
    // Search for map in array (yes I know there is a binary search but the dataset is really
    // small and unsorted so this is okay to do)
    mapinfo* selectedMap = nullptr;
    for (int i = 0; i < mapCount; i++) {
        if (maps[i].name == map) {
            selectedMap = maps + i;
        }
    }

    if (selectedMap == nullptr) {
        return nullptr;
    }

    // allocate new map info
    loadedMap* returnMap = new loadedMap();

    // load map info
    returnMap->map = selectedMap;
    returnMap->mapTexture.loadFromFile(selectedMap->radarName);
    returnMap->mapSprite = sf::Sprite(returnMap->mapTexture);

    if (selectedMap->hasTwoLayers) {
        window.setSize(sf::Vector2u(2048, 1024));
        window.setView(sf::View(sf::Vector2f(1024, 512), sf::Vector2f(2048, 1024)));
        returnMap->mapTextureLower.loadFromFile(selectedMap->lowerLayerName);
        returnMap->mapSpriteLower = sf::Sprite(returnMap->mapTextureLower);
        returnMap->mapSpriteLower.setPosition(sf::Vector2f(1024, 0));
    }
    else {
        window.setSize(sf::Vector2u(1024, 1024));
        window.setView(sf::View(sf::Vector2f(512, 512), sf::Vector2f(1024, 1024)));
    }

    return returnMap;
}