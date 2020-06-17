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

#include <json.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

#include "map.h"

#include <memory>

enum class grenadeType {
	inferno,
	frag,
	smoke,
	flashbang,
	decoy
};

struct grenade {
	float lifetime;
	float effecttime;
	sf::Vector3f position;
	bool fromCT;

	virtual grenadeType type() = 0;
	virtual void render(sf::RenderWindow& window, const mapinfo& mapinfo) = 0;
};

struct infernoGrenade : grenade {
	grenadeType type() override { return grenadeType::inferno; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

struct fragGrenade : grenade {
	grenadeType type() override { return grenadeType::inferno; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

struct smokeGrenade : grenade {
	grenadeType type() override { return grenadeType::inferno; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

struct flashbangGrenade : grenade {
	grenadeType type() override { return grenadeType::inferno; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

struct decoyGrenade : grenade {
	grenadeType type() override { return grenadeType::inferno; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

std::vector<std::shared_ptr<grenade>> processGrenades(nlohmann::json info, bool* debug);