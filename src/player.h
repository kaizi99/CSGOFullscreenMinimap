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
#include "json.hpp"

#include "map.h"

struct player {
    sf::Vector3f position;
    sf::Vector2f minimapPosition;
    bool isOnLowerLevel;
    int observerSlot;
    bool isCT;
    bool dead;
    float rotation;
    std::string steamID;
    sf::Text playerNameText;

    player(nlohmann::json playerJson, std::string steamid, sf::Font& playerFont, loadedMap* loadedMap);

    player interpolate(player b, float t);
};