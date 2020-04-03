#include "interpolation.h"
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

#include "interpolation.h"

#include <unordered_map>
#include <unordered_set>
#include <iostream>

interpolation::interpolation(csgo_gamestate* gs)
{
	gs->add_callback([this](nlohmann::json gs) { 
		arrivedGamestateMutex.lock();

		newGamestateArrived = true;
		arrivedGamestate = gs;

		arrivedGamestateMutex.unlock();
	});

	playerFont.loadFromFile("Roboto-Regular.ttf");
}

interpolation::~interpolation()
{

}

// There is something completely wrong with this, it interpolated between the first ever and the newest gamestate
// and I don't know whats wrong but I'm not in the mood for debugging right now, so feel free to debug this yourself
// Disclaimer: I have never done interpolation so there might be a better way to organize this
std::vector<player> interpolation::processInterpolation()
{
	nlohmann::json arrivedGS;

	// Access synchronized gamestate variables
	arrivedGamestateMutex.lock();
	
	bool newGamestate = newGamestateArrived;
	
	if (newGamestate) {
		arrivedGS = arrivedGamestate;
		newGamestateArrived = false;
		gamestateDelta = clockSinceLastGamestate.getElapsedTime();
		clockSinceLastGamestate.restart();
	}

	arrivedGamestateMutex.unlock();

	// Calculate current gamestate
	float t = (clockSinceLastGamestate.getElapsedTime() / gamestateDelta);

	if (t > 1.0f) {
		t = 1.0f;
	}

	std::vector<player> currentGS;

	std::unordered_map<std::string, player*> newGamestateMap;
	std::unordered_set<std::string> alreadyAddedPlayers;

	for (player& p : currentGamestate) {
		newGamestateMap[p.steamID] = &p;
	}

	// Interpolate all players that are part of the new and old gamestate
	for (auto& oldPlayerState : oldGamestate) {
		auto newPlayerFound = newGamestateMap.find(oldPlayerState.steamID);

		if (newPlayerFound == newGamestateMap.end()) {
			continue;
		}

		player newPlayerState = *newPlayerFound->second;

		currentGS.push_back(oldPlayerState.interpolate(newPlayerState, t));

		alreadyAddedPlayers.insert(oldPlayerState.steamID);
	}

	// Since new players that are not part of the old gamestate are ignored in the algorithm
	// above we have to add them manually
	for (auto& newPlayerState : currentGamestate) {
		if (alreadyAddedPlayers.find(newPlayerState.steamID) == alreadyAddedPlayers.end()) {
			currentGS.push_back(newPlayerState);
		}
	}

	// If there is a new gamestate, set current gamestate as old and the new gamestate as current
	if (newGamestate) {
		gamestateDelta = clockSinceLastGamestate.getElapsedTime();
		clockSinceLastGamestate.restart();
		oldGamestate = currentGS;

		currentGamestate = std::vector<player>();

		for (auto p : arrivedGS["allplayers"].items()) {
			currentGamestate.push_back(player(p.value(), p.key(), playerFont, currentlyLoadedMap));
		}
	}

	return currentGS;
}
