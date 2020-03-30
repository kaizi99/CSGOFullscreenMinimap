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

#include "imgui_sfml.h"

#include <iostream>

#include <SFML/OpenGL.hpp>

void imgui_sfml_init(const std::string& font)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(font.c_str(), 13.0f);
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = false;

	sf::Texture* tex = new sf::Texture();

	int width, height;
	unsigned char* pixels = NULL;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	tex->create(width, height);
	tex->update(pixels);

	io.Fonts->TexID = tex;

	io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
	io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
	io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
	io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
	io.KeyMap[ImGuiKey_PageUp] = sf::Keyboard::PageUp;
	io.KeyMap[ImGuiKey_PageDown] = sf::Keyboard::PageDown;
	io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
	io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
	io.KeyMap[ImGuiKey_Insert] = sf::Keyboard::Insert;
	io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
	io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::Backspace;
	io.KeyMap[ImGuiKey_Space] = sf::Keyboard::Space;
	io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Enter;
	io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
	io.KeyMap[ImGuiKey_KeyPadEnter] = sf::Keyboard::Enter;
	io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
	io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
	io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
	io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
	io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
	io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;
}

void imgui_sfml_process_event(const sf::Event& event)
{
	ImGuiIO& io = ImGui::GetIO();
	
	if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
	{
		int key = event.key.code;
		io.KeysDown[key] = (event.type == sf::Event::KeyPressed);
		io.KeyShift = event.key.shift;
		io.KeyCtrl = event.key.control;
		io.KeyAlt = event.key.alt;
		io.KeySuper = event.key.system;
	}

	if (event.type == sf::Event::TextEntered)
	{
		sf::String str(event.text.unicode);
		io.AddInputCharactersUTF8((const char*)str.toUtf8().c_str());
	}

	if (event.type == sf::Event::MouseWheelMoved)
	{
		io.MouseWheel = event.mouseWheel.delta;
	}
}

void imgui_sfml_begin_frame(const sf::RenderWindow& window, float deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();
	//io.DeltaTime = deltaTime;
	io.DisplaySize.x = window.getSize().x;
	io.DisplaySize.y = window.getSize().y;

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);

	io.MousePos.x = mousePos.x;
	io.MousePos.y = mousePos.y;
	io.MouseDown[0] = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	io.MouseDown[1] = sf::Mouse::isButtonPressed(sf::Mouse::Right);

	ImGui::NewFrame();
	//ImGui::DockSpaceOverViewport();
}

void imgui_sfml_end_frame(sf::RenderWindow& window)
{
	ImGui::EndFrame();
	ImGui::Render();

	ImDrawData* draw_data = ImGui::GetDrawData();

	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			sf::Texture* texture = (sf::Texture*)pcmd->TextureId;

			std::vector<sf::Vertex> vertices;
			vertices.reserve(pcmd->ElemCount);

			for (int i = 0; i < pcmd->ElemCount; i++)
			{
				const ImDrawVert vert = vtx_buffer[idx_buffer[i]];

				sf::Color col(vert.col & 0x000000FF, (vert.col & 0x0000FF00) >> 8, (vert.col & 0x00FF0000) >> 16, (vert.col & 0xFF000000) >> 24);

				vertices.push_back(sf::Vertex(sf::Vector2f(vert.pos.x, vert.pos.y), col, sf::Vector2f(vert.uv.x * texture->getSize().x, vert.uv.y * texture->getSize().y)));
			}

			sf::VertexBuffer vbuffer(sf::PrimitiveType::Triangles);
			vbuffer.create(pcmd->ElemCount);
			vbuffer.update(vertices.data(), pcmd->ElemCount, 0);

			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				sf::RenderStates state;
				state.texture = texture;

				glEnable(GL_SCISSOR_TEST);

				// Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

				int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    			int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);

				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
					bool clip_origin_lower_left = true;
                    // Apply scissor/clipping rectangle
                    if (clip_origin_lower_left)
                        glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
                    else
                        glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z, (int)clip_rect.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)

					window.draw(vbuffer, state);
				}
				glDisable(GL_SCISSOR_TEST);

				ImVec2 pos = draw_data->DisplayPos;
			}

			idx_buffer += pcmd->ElemCount;
		}
	}
}

void imgui_sfml_destroy()
{
	ImGui::DestroyContext();
}