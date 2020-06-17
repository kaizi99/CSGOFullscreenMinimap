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

std::unordered_map<std::string, std::shared_ptr<grenade>> processGrenades(nlohmann::json info, bool* debug) {
	if (debug) {
		if (ImGui::Begin("Grenade Debugging", debug)) {
			ImGui::TextWrapped("%s", info["grenades"].dump(4).c_str());
		}

		ImGui::End();
	}

	std::unordered_map<std::string, std::shared_ptr<grenade>> grenadeMap;

	for (const auto& g : info["grenades"].items()) {
	    std::string type = g.value()["type"];

	    if (type == "inferno" || type == "firebomb") {
            grenadeMap.emplace(g.key(), std::make_shared<infernoGrenade>(g.value()));
	    } else if (type == "smoke") {
            grenadeMap.emplace(g.key(), std::make_shared<smokeGrenade>(g.value()));
	    } else if (type == "flashbang") {
            grenadeMap.emplace(g.key(), std::make_shared<flashbangGrenade>(g.value()));
	    } else if (type == "decoy") {
            grenadeMap.emplace(g.key(), std::make_shared<decoyGrenade>(g.value()));
	    } else {
	        std::cout << "Unkown grenade: " << type << std::endl;
	    }
	}

	return grenadeMap;
}

grenade::grenade(float Lifetime, float EffectTime, sf::Vector3f Position, bool FromCT)
    : lifetime(Lifetime), effecttime(EffectTime), position(Position), fromCT(FromCT) {}

void infernoGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo) {

}

infernoGrenade::infernoGrenade(const nlohmann::json &info)
    : grenade(info["lifetime"], info["effecttime"], sf::Vector3f(), true) {

    sscanf(info["position"].get<std::string>().c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);
}

void fragGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo) {

}

fragGrenade::fragGrenade(const nlohmann::json &info)
    : grenade(info["lifetime"], info["effecttime"], sf::Vector3f(), true) {
    sscanf(info["position"].get<std::string>().c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);
}

void smokeGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo) {

}

smokeGrenade::smokeGrenade(const nlohmann::json &info)
    : grenade(info["lifetime"], info["effecttime"], sf::Vector3f(), true) {
    sscanf(info["position"].get<std::string>().c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);
}

void flashbangGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo) {

}

flashbangGrenade::flashbangGrenade(const nlohmann::json &info)
    : grenade(info["lifetime"], info["effecttime"], sf::Vector3f(), true) {
    sscanf(info["position"].get<std::string>().c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);
}

void decoyGrenade::render(sf::RenderWindow &window, const mapinfo &mapinfo) {

}

decoyGrenade::decoyGrenade(const nlohmann::json &info)
    : grenade(info["lifetime"], info["effecttime"], sf::Vector3f(), true) {
    sscanf(info["position"].get<std::string>().c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);
}
