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

#include "player.h"

#define M_PI 3.14159265358979323846
#define M_PI_2 	1.57079632679489661923
#include <cmath>

player::player(nlohmann::json playerJson, std::string steamid, sf::Font& playerFont, loadedMap* loadedMap)
{
    // Determine player 3D position
    std::string positionString = playerJson["position"];
    sscanf(positionString.c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);

    steamID = steamid;

    // Calculate the position on the minimap based on the 3D position
    sf::Vector3f minimapPosition3D = loadedMap->map->upperLeft - position;
    minimapPosition = sf::Vector2f((-minimapPosition3D.x / loadedMap->map->scale), (minimapPosition3D.y / loadedMap->map->scale));

    if (loadedMap->map->hasTwoLayers && minimapPosition3D.z > loadedMap->map->cutoff) {
        isOnLowerLevel = true;
    }
    else {
        isOnLowerLevel = false;
    }

    // Determine observer slot
    if (!playerJson["observer_slot"].is_null()) {
        observerSlot = playerJson["observer_slot"].get<int>();
    }
    else {
        observerSlot = -1;
    }

    // Determine team
    std::string team = playerJson["team"];
    if (team == "T") {
        isCT = false;
    }
    else {
        isCT = true;
    }

    // Determine alive status
    if (playerJson["state"]["health"].get<int>() == 0) {
        dead = true;
    }
    else {
        dead = false;
    }

    // Setup player name text
    playerNameText.setString(playerJson["name"].get<std::string>());
    playerNameText.setFillColor(isCT ? sf::Color::Cyan : sf::Color::Yellow);
    playerNameText.setOutlineColor(sf::Color::Black);
    playerNameText.setOutlineThickness(1);
    playerNameText.setCharacterSize(dead ? 12 : 15);
    playerNameText.setFont(playerFont);
    sf::FloatRect localBounds = playerNameText.getLocalBounds();
    playerNameText.setOrigin(floor(playerNameText.getLocalBounds().width), floor(playerNameText.getLocalBounds().height));

    // Calculate rotation from 3D forward vector
    // There is probably a more clever way to do this but I came up with it and it works.
    std::string forwardString = playerJson["forward"];
    sf::Vector3f forward;
    sscanf(forwardString.c_str(), "%f, %f, %f", &forward.x, &forward.y, &forward.z);

    float angle;
    if (forward.x > 0) {
        angle = acos(forward.y);
    }
    else if (forward.x < 0) {
        angle = asin(forward.y) + M_PI + M_PI_2;
    }
    else {
        if (forward.y > 0) {
            angle = 0;
        }
        else {
            angle = M_PI;
        }
    }

    rotation = (angle / (M_PI * 2)) * 360;
}

sf::Vector3f interpolateVector(const sf::Vector3f& a, const sf::Vector3f b, float t) {
    return (1.0f - t) * a + t * b;
}

// Since the interpolation class is broken the interpolation is currenlty "disabled", see interpolation::processInterpolation();
player player::interpolate(player b, float t)
{
    /*
    if (t > 1.0f) {
        t = 1.0f;
    }

    b.position = interpolateVector(position, b.position, t);
    b.minimapPosition = (1.0f - t) * minimapPosition + t * b.minimapPosition;
    b.rotation = (1.0f - t) * rotation + t * b.rotation;

    */
    return b;
}
