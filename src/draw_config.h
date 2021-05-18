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

#include <vector>
#include "json.hpp"

struct draw_config {
    std::string name;
    bool drawTwoMaps;
    bool drawName;
    int nameCharacterSize;
    int nameDeadCharacterSize;
    float circleSize;
    float circleOutlineSize;
    int observerTextSize;
    float bombIconScale;
    bool drawBauchbinde;
};

std::vector<draw_config> draw_config_parse_json(nlohmann::json input);
nlohmann::json draw_config_to_json(std::vector<draw_config> configs);