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
#include <unordered_map>
#include <memory>

#include "map.h"

enum class grenadeType {
	inferno,
	frag,
	smoke,
	flashbang,
	decoy
};

struct grenade {
    grenade(float Lifetime, float EffectTime, sf::Vector3f Position, bool FromCT);

	float lifetime;
	float effecttime;
	sf::Vector3f position;
	bool fromCT;

	virtual grenadeType type() = 0;
	virtual void render(sf::RenderWindow& window, const mapinfo& mapinfo) = 0;
};

struct infernoGrenade : grenade {
    infernoGrenade(const nlohmann::json& info);

	grenadeType type() override { return grenadeType::inferno; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

struct fragGrenade : grenade {
    fragGrenade(const nlohmann::json& info);

	grenadeType type() override { return grenadeType::frag; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

struct smokeGrenade : grenade {
    smokeGrenade(const nlohmann::json& info);

	grenadeType type() override { return grenadeType::smoke; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

struct flashbangGrenade : grenade {
    flashbangGrenade(const nlohmann::json& info);

	grenadeType type() override { return grenadeType::flashbang; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

struct decoyGrenade : grenade {
    decoyGrenade(const nlohmann::json& info);

	grenadeType type() override { return grenadeType::decoy; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo) override;
};

std::unordered_map<std::string, std::shared_ptr<grenade>> processGrenades(nlohmann::json info, bool* debug);