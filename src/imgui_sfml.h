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

#include <imgui.h>
#include <SFML/Graphics.hpp>

#include <string>

void imgui_sfml_init(const std::string& font);
void imgui_sfml_process_event(const sf::Event& event);
void imgui_sfml_begin_frame(const sf::RenderWindow& window, float deltaTime);
void imgui_sfml_end_frame(sf::RenderWindow& window);
void imgui_sfml_destroy();

