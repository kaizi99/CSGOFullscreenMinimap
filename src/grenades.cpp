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

std::vector<std::shared_ptr<grenade>> processGrenades(nlohmann::json info, bool* debug) {
	if (debug) {
		if (ImGui::Begin("Grenade Debugging", debug)) {
			ImGui::TextWrapped("%s", info["grenades"].dump(4).c_str());
		}

		ImGui::End();
	}

	return std::vector<std::shared_ptr<grenade>>();
}