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


#include "bomb.h"

#include <iostream>

bomb::bomb(nlohmann::json bombJson, loadedMap* map) {
	if (!bombJson.is_null()) {
		stateString = bombJson["state"];

		if (stateString == "carried") {
			state = bomb_state::CARRIED;
			carrierID = std::to_string(bombJson["player"].get<long long int>());
		}
		else if (stateString == "dropped") {
			state = bomb_state::DROPPED;
		}
		else if (stateString == "defused") {
			state = bomb_state::DEFUSED;
		}
		else {
			state = bomb_state::PLANTED;
		}

		std::string positionString = bombJson["position"];
		sscanf(positionString.c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);

		// Calculate the position on the minimap based on the 3D position
		sf::Vector3f minimapPosition3D = map->map.upperLeft - position;
		minimapPosition = sf::Vector2f((-minimapPosition3D.x / map->map.scale), (minimapPosition3D.y / map->map.scale));

		if (map->map.hasTwoLayers && minimapPosition3D.z > map->map.cutoff) {
			isOnLowerLevel = true;
		}
		else {
			isOnLowerLevel = false;
		}
	}
	else {
		state = bomb_state::UNDEFINED;
	}
}