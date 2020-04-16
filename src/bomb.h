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
#include <json.hpp>

#include "map.h"

enum class bomb_state {
	UNDEFINED,
	CARRIED,
	DROPPED,
	PLANTED,
	DEFUSED
};

struct bomb {
	sf::Vector3f position;

	bomb_state state;

	std::string stateString;
	std::string carrierID;

	sf::Vector2f minimapPosition;
	bool isOnLowerLevel;

	bomb(nlohmann::json bombJson, loadedMap* map);
};