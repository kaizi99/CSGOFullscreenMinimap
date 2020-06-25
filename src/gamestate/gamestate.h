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

#include <SFML/Graphics.hpp>

enum class gamestate_team {
	T,
	CT
};

enum class gamestate_bomb_state {
	UNDEFINED,
	CARRIED,
	DROPPED,
	PLANTED,
	DEFUSED
};

enum class gamestate_grenade_type {
	INFERNO,
	FIREBOMB,
	FRAG,
	SMOKE,
	FLASHBANG,
	DECOY
};

struct gamestate_player {
	std::string steamID;
	std::string name;
	int observerSlot;

	float rotationAngle;
	sf::Vector3f position;
	gamestate_team team;
};

struct gamestate_bomb {
	sf::Vector3f position;

	bomb_state state;

	std::string stateString;
	std::string carrierID;
};

struct gamestate_grenade {
	gamestate_grenade_type grenadeType;

	float lifetime;

	bool hasPosition;
	sf::Vector3f position;

	bool hasEffectTime;
	float effectTime;

	//TODO: Find out if actually 10 flames
	sf::Vector3f flames[10];
};

struct gamestate {
	gamestate_player players[10];
	gamestate_bomb bomb;
	gamestate_grenade grenades[40];
	std::string map;
};