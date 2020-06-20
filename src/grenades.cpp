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

#include "grenades.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <iostream>

std::unordered_map<std::string, std::shared_ptr<grenade>> processGrenades(nlohmann::json info, std::shared_ptr<grenadeResources> resources, const std::vector<player>& players) {
	std::unordered_map<std::string, bool> steamIDToTeam;
	for (const auto& p : players) {
		steamIDToTeam[p.steamID] = p.isCT;
	}

	std::unordered_map<std::string, std::shared_ptr<grenade>> grenadeMap;

	for (const auto& g : info["grenades"].items()) {
	    std::string type = g.value()["type"];

		std::string ownerString = std::to_string(g.value()["owner"].get<long long int>());
		bool fromCT = steamIDToTeam[ownerString];

	    if (type == "inferno" || type == "firebomb") {
            grenadeMap.emplace(g.key(), std::make_shared<infernoGrenade>(g.value(), sf::Sprite(resources->molotov), sf::Sprite(resources->incgrenade), fromCT));
		}
		else if (type == "smoke") {
			grenadeMap.emplace(g.key(), std::make_shared<smokeGrenade>(g.value(), sf::Sprite(resources->smokegrenade), fromCT));
		}
		else if (type == "frag") {
			grenadeMap.emplace(g.key(), std::make_shared<fragGrenade>(g.value(), sf::Sprite(resources->hegrenade), fromCT));
		}
		else if (type == "flashbang") {
            grenadeMap.emplace(g.key(), std::make_shared<flashbangGrenade>(g.value(), sf::Sprite(resources->flashbang), fromCT));
	    } 
		else if (type == "decoy") {
            grenadeMap.emplace(g.key(), std::make_shared<decoyGrenade>(g.value(), sf::Sprite(resources->decoy), fromCT));
	    } 
		else {
	        std::cout << "Unkown grenade: " << type << std::endl;
	    }
	}

	return grenadeMap;
}

grenade::grenade(const nlohmann::json& info, sf::Sprite sprite, bool fromCT)
	: hasPosition(false), hasEffectTime(false), sprite(sprite), fromCT(fromCT) {
	lifetime = std::stof(info["lifetime"].get<std::string>(), nullptr);

	if (info.find("position") != info.end()) {
		hasPosition = true;
		sscanf(info["position"].get<std::string>().c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);
	}

	if (info.find("effecttime") != info.end()) {
		hasEffectTime = true;
		effectTime = std::stof(info["effecttime"].get<std::string>(), nullptr);
	}
}

sf::Vector2f grenade::translateToMapSpace(sf::Vector3f pos, const mapinfo& mapinfo, const draw_config& drawConfig) {
	sf::Vector3f minimapPosition3D = mapinfo.upperLeft - pos;
	sf::Vector2f minimapPosition = sf::Vector2f((-minimapPosition3D.x / mapinfo.scale), (minimapPosition3D.y / mapinfo.scale));

	if (minimapPosition3D.z > mapinfo.cutoff && drawConfig.drawTwoMaps) {
		minimapPosition += mapinfo.lowerLayerOffset;
	}

	return minimapPosition;
}

void infernoGrenade::render(sf::RenderWindow& window, const mapinfo& mapinfo, const draw_config& drawConfig) {
	if (position == sf::Vector3f()) {
		sf::CircleShape flameShape(5.0f);
		flameShape.setFillColor(fromCT ? sf::Color::Cyan : sf::Color::Yellow);

		for (const auto& flame : flames) {
			sf::Vector2f pos = translateToMapSpace(flame, mapinfo, drawConfig);
			flameShape.setPosition(pos);
			window.draw(flameShape);
		}
	}
	else {
		sf::Sprite s = isFirebomb ? firebombSprite : sprite;
		s.setOrigin(s.getLocalBounds().width / 2, s.getLocalBounds().height / 2);
		s.setScale(0.2f, 0.2f);
		s.setPosition(translateToMapSpace(position, mapinfo, drawConfig));
		s.setColor(fromCT ? sf::Color::Cyan : sf::Color::Yellow);
		window.draw(s);
	}
}

infernoGrenade::infernoGrenade(const nlohmann::json& info, sf::Sprite sprite, sf::Sprite firebombSprite, bool fromCT)
	: grenade(info, sprite, fromCT), firebombSprite(firebombSprite) {

	if (info["type"] == "firebomb") {
		isFirebomb = true;
	}
	else {
		isFirebomb = false;
	}
  
	if (info.find("flames") != info.end()) {
		for (const auto& f : info["flames"].items()) {
			sf::Vector3f flame;
			sscanf(f.value().get<std::string>().c_str(), "%f, %f, %f", &flame.x, &flame.y, &flame.z);

			flames.push_back(flame);
		}
	}
}

void fragGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo, const draw_config& drawConfig) {
	if (lifetime <= 3.0f) {
		sf::Sprite s = sprite;
		s.setOrigin(s.getLocalBounds().width / 2, s.getLocalBounds().height / 2);
		s.setScale(0.2f, 0.2f);
		s.setPosition(translateToMapSpace(position, mapinfo, drawConfig));
		s.setColor(fromCT ? sf::Color::Cyan : sf::Color::Yellow);
		window.draw(s);
	}
}

fragGrenade::fragGrenade(const nlohmann::json &info, sf::Sprite sprite, bool fromCT)
	: grenade(info, sprite, fromCT) {

}

void smokeGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo, const draw_config& drawConfig) {
	if (hasEffectTime && effectTime > 0) {
		sf::CircleShape smokeShape(23.0f);
		smokeShape.setOrigin(23.0f, 23.0f);
		sf::Color fillColor = fromCT ? sf::Color::Cyan : sf::Color::Yellow;
		smokeShape.setFillColor(fillColor - sf::Color(0, 0, 0, 50));
		smokeShape.setPosition(translateToMapSpace(position, mapinfo, drawConfig));
		window.draw(smokeShape);
	}
	else {
		sf::Sprite s = sprite;
		s.setOrigin(s.getLocalBounds().width / 2, s.getLocalBounds().height / 2);
		s.setScale(0.2f, 0.2f);
		s.setPosition(translateToMapSpace(position, mapinfo, drawConfig));
		s.setColor(fromCT ? sf::Color::Cyan : sf::Color::Yellow);
		window.draw(s);
	}
}

smokeGrenade::smokeGrenade(const nlohmann::json &info, sf::Sprite sprite, bool fromCT)
	: grenade(info, sprite, fromCT) {

}

void flashbangGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo, const draw_config& drawConfig) {
	sf::Sprite s = sprite;
	s.setOrigin(s.getLocalBounds().width / 2, s.getLocalBounds().height / 2);
	s.setScale(0.2f, 0.2f);
	s.setPosition(translateToMapSpace(position, mapinfo, drawConfig));
	s.setColor(fromCT ? sf::Color::Cyan : sf::Color::Yellow);
	window.draw(s);
}

flashbangGrenade::flashbangGrenade(const nlohmann::json &info, sf::Sprite sprite, bool fromCT)
	: grenade(info, sprite, fromCT) {

}

void decoyGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo, const draw_config& drawConfig) {
	sf::Sprite s = sprite;
	s.setOrigin(s.getLocalBounds().width / 2, s.getLocalBounds().height / 2);
	s.setScale(0.2f, 0.2f);
	s.setPosition(translateToMapSpace(position, mapinfo, drawConfig));
	s.setColor(fromCT ? sf::Color::Cyan : sf::Color::Yellow);
	window.draw(s);
}

decoyGrenade::decoyGrenade(const nlohmann::json &info, sf::Sprite sprite, bool fromCT)
	: grenade(info, sprite, fromCT) {

}
