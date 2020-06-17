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
#include <string>

#include "map.h"
#include "draw_config.h"
#include "player.h"

struct grenadeResources {
	sf::Texture decoy;
	sf::Texture flashbang;
	sf::Texture hegrenade;
	sf::Texture incgrenade;
	sf::Texture molotov;
	sf::Texture smokegrenade;
};

enum class grenadeType {
	inferno,
	frag,
	smoke,
	flashbang,
	decoy
};

struct grenade {
    grenade(const nlohmann::json& info, sf::Sprite sprite, bool fromCT);

	float lifetime;

	bool hasPosition;
	sf::Vector3f position;

	bool hasEffectTime;
	float effectTime;

	bool fromCT;

	sf::Sprite sprite;

	sf::Vector2f translateToMapSpace(sf::Vector3f pos, const mapinfo& mapinfo, const draw_config& drawConfig);

	virtual grenadeType type() = 0;
	virtual void render(sf::RenderWindow& window, const mapinfo& mapinfo, const draw_config& drawConfig) = 0;
};

struct infernoGrenade : grenade {
	std::vector<sf::Vector3f> flames;
	bool isFirebomb;
	sf::Sprite firebombSprite;

    infernoGrenade(const nlohmann::json& info, sf::Sprite sprite, sf::Sprite firebombSprite, bool fromCT);

	grenadeType type() override { return grenadeType::inferno; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo, const draw_config& drawConfig) override;
};

struct fragGrenade : grenade {
    fragGrenade(const nlohmann::json& info, sf::Sprite sprite, bool fromCT);

	grenadeType type() override { return grenadeType::frag; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo, const draw_config& drawConfig) override;
};

struct smokeGrenade : grenade {
    smokeGrenade(const nlohmann::json& info, sf::Sprite sprite, bool fromCT);

	grenadeType type() override { return grenadeType::smoke; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo, const draw_config& drawConfig) override;
};

struct flashbangGrenade : grenade {
    flashbangGrenade(const nlohmann::json& info, sf::Sprite sprite, bool fromCT);

	grenadeType type() override { return grenadeType::flashbang; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo, const draw_config& drawConfig) override;
};

struct decoyGrenade : grenade {
    decoyGrenade(const nlohmann::json& info, sf::Sprite sprite, bool fromCT);

	grenadeType type() override { return grenadeType::decoy; }
	void render(sf::RenderWindow& window, const mapinfo& mapinfo, const draw_config& drawConfig) override;
};

std::unordered_map<std::string, std::shared_ptr<grenade>> processGrenades(nlohmann::json info, bool* debug, std::shared_ptr<grenadeResources> resources, const std::vector<player>& players);