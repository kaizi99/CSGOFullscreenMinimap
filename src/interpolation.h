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

#include <mutex>
#include "player.h"
#include "map.h"
#include "csgo_gamestate.h"

class interpolation {
public:
	interpolation(csgo_gamestate* gs);
	~interpolation();

	std::vector<player> processInterpolation();

	loadedMap* currentlyLoadedMap;

private:
	sf::Font playerFont;

	sf::Clock clockSinceLastGamestate;
	sf::Time gamestateDelta;

	std::vector<player> oldGamestate;
	std::vector<player> currentGamestate;

	// We need to synchronize access to the arrived gamestate variabled
	// because the gamestate server lives inside another thread
	std::mutex arrivedGamestateMutex;

	bool newGamestateArrived;
	nlohmann::json arrivedGamestate;
};