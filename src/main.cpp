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

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <algorithm>

#include "imgui_sfml.h"
#include "csgo_gamestate.h"
#include "player.h"
#include "map.h"
#include "interpolation.h"
#include "draw_config.h"

int main()
{
    // Hardcoded maps, this will be replaced by an ImGui interface and saved json
    mapinfo maps[7];
    maps[0].name = "de_overpass";
    maps[0].radarName = "de_overpass_radar.png";
    maps[0].upperLeft = sf::Vector3f(-4831, 1781, 0);
    maps[0].scale = 5.2f;
    maps[0].hasTwoLayers = false;

    maps[1].name = "de_nuke";
    maps[1].radarName = "de_nuke_radar.png";
    maps[1].upperLeft = sf::Vector3f(-3453, 2887, 0);
    maps[1].scale = 7;
    maps[1].hasTwoLayers = true;
    maps[1].lowerLayerName = "de_nuke_lower_radar.png";
    maps[1].cutoff = 495;

    maps[2].name = "de_vertigo";
    maps[2].radarName = "de_vertigo_radar.png";
    maps[2].upperLeft = sf::Vector3f(-3168, 1762, 0);
    maps[2].scale = 4;
    maps[2].hasTwoLayers = true;
    maps[2].lowerLayerName = "de_vertigo_lower_radar.png";
    maps[2].cutoff = -11700;

    maps[3].name = "de_dust2";
    maps[3].radarName = "de_dust2_radar.png";
    maps[3].upperLeft = sf::Vector3f(-2476, 3239, 0);
    maps[3].scale = 4.4f;
    maps[3].hasTwoLayers = false;

    maps[4].name = "de_inferno";
    maps[4].radarName = "de_inferno_radar.png";
    maps[4].upperLeft = sf::Vector3f(-2087, 3870, 0);
    maps[4].scale = 4.9f;
    maps[4].hasTwoLayers = false;

    maps[5].name = "de_mirage";
    maps[5].radarName = "de_mirage_radar.png";
    maps[5].upperLeft = sf::Vector3f(-3230, 1713, 0);
    maps[5].scale = 5;
    maps[5].hasTwoLayers = false;

    maps[6].name = "de_train";
    maps[6].radarName = "de_train_radar.png";
    maps[6].upperLeft = sf::Vector3f(-2477, 2392, 0);
    maps[6].scale = 4.7f;
    maps[6].hasTwoLayers = false;

#define OBSERVER 0
#define MINIMAP 1

    draw_config configs[2];
    configs[OBSERVER].drawTwoMaps = true;
    configs[OBSERVER].drawName = true;
    configs[OBSERVER].nameCharacterSize = 15;
    configs[OBSERVER].nameDeadCharacterSize = 12;
    configs[OBSERVER].circleSize = 10;
    configs[OBSERVER].observerTextSize = 12;

    configs[MINIMAP].drawTwoMaps = false;
    configs[MINIMAP].drawName = false;
    configs[MINIMAP].circleSize = 20;
    configs[MINIMAP].observerTextSize = 23;

    draw_config activeConfig = configs[OBSERVER];

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1024, 1024), "Fullscreen CSGO Map by kaizi99");
    window.setFramerateLimit(60);
    
    // Setup the cross sprite
    sf::Texture cross;
    cross.loadFromFile("cross.png");
    sf::Sprite crossSprite(cross);
    crossSprite.setOrigin(5, 5);
    crossSprite.setScale(sf::Vector2f(0.75, 0.75));

    // Setup the direction triangle
    sf::CircleShape triangle(activeConfig.circleSize, 3);
    triangle.setFillColor(sf::Color::White);
    triangle.setOutlineColor(sf::Color::Black);
    triangle.setOutlineThickness(1);
    triangle.setOrigin(activeConfig.circleSize, activeConfig.circleSize * 2);

    // Load the observer slot and player name fonts
    sf::Font observerSlotFont;
    observerSlotFont.loadFromFile("Roboto-Regular.ttf");
    sf::Font playerNameFont;
    playerNameFont.loadFromFile("Roboto-Regular.ttf");

    // Setup the observer slot texts
    sf::Text observerSlotTexts[10];
    for (int i = 0; i < 10; i++) {
        observerSlotTexts[i].setCharacterSize(activeConfig.observerTextSize);
        observerSlotTexts[i].setStyle(sf::Text::Bold);
        observerSlotTexts[i].setString(std::to_string(i));
        observerSlotTexts[i].setFillColor(sf::Color::Black);
        observerSlotTexts[i].setFont(observerSlotFont);
        observerSlotTexts[i].setOrigin(floor(observerSlotTexts[i].getLocalBounds().width / 2), floor(observerSlotTexts[i].getLocalBounds().height / 2));
    }

    // Setup the player circle
    sf::CircleShape playerCircle;
    playerCircle.setFillColor(sf::Color::Cyan);
    playerCircle.setOutlineColor(sf::Color::Black);
    playerCircle.setOutlineThickness(2);
    playerCircle.setRadius(activeConfig.circleSize);
    playerCircle.setOrigin(activeConfig.circleSize, activeConfig.circleSize);

    // Initialize the ImGui and the SFML integration
    imgui_sfml_init("Roboto-Regular.ttf");

    // Start the csgo gamestate http server on port 1338
    csgo_gamestate* gamestate = new csgo_gamestate(1338);
    interpolation interp(gamestate);

    loadedMap* loadedMap = nullptr;

    bool drawImGUI = true;
    bool enableInterpolation = true;

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::T) {
                    drawImGUI = !drawImGUI;
                }
            }

            imgui_sfml_process_event(event);
        }

        imgui_sfml_begin_frame(window, 0.1f);

        // Clear screen
        window.clear(sf::Color(0, 0, 0, 0));

        auto gs = gamestate->get_latest_gamestate();

        // Only draw stuff if there is a supported map
        if (loadedMap != nullptr) {
            std::vector<player> players;
            if (enableInterpolation) {
                // Get all players interpolated
                players = interp.processInterpolation();
            } else {
                //std::cout << gamestate->get_latest_gamestate() << std::endl;
                for (auto p : gs["allplayers"].items()) {
                    //std::cout << p.value() << std::endl;
                    players.push_back(player(p.value(), p.key(), gs["player"], observerSlotFont, loadedMap));
                }
            }


            // Sort all dead players under the alive players to draw the alive player always above dead players
            std::sort(players.begin(), players.end(), [](player a, player b) { return a.dead && !b.dead; });
            
            // Find the first alive player in the list
            std::vector<player>::iterator it;
            for (it = players.begin(); it != players.end(); ++it) {
                if (!it->dead) {
                    break;
                }
            }

            // Sort all alive players based on height in the map to draw boosted players over the boosters and dead players
            std::sort(it, players.end(), [](player a, player b) { return a.position.z < b.position.z; });

            bool currenltyObserverdIsLower = false;
            for (auto p : players) {
                if (p.currentlyObserved && p.isOnLowerLevel) {
                    currenltyObserverdIsLower = true;
                }
            }

            // Draw the overviews
            if (loadedMap->map->hasTwoLayers) {
                if (activeConfig.drawTwoMaps) {
                    window.draw(loadedMap->mapSprite);
                    window.draw(loadedMap->mapSpriteLower);
                }
                else {
                    window.draw(currenltyObserverdIsLower ? loadedMap->mapSpriteLower : loadedMap->mapSprite);
                }
            }
            else {
                window.draw(loadedMap->mapSprite);
            }

            // Draw each player
            for (auto p : players) {
                if (!p.dead) {
                    // Draw the player's rotation and player circle if he isn't dead
                    triangle.setRotation(p.rotation);

                    if (p.isOnLowerLevel && activeConfig.drawTwoMaps) {
                        playerCircle.setPosition(sf::Vector2f(p.minimapPosition.x + 1024, p.minimapPosition.y));
                        triangle.setPosition(sf::Vector2f(p.minimapPosition.x + 1024, p.minimapPosition.y));
                    }
                    else {
                        playerCircle.setPosition(p.minimapPosition);
                        triangle.setPosition(p.minimapPosition);
                    }

                    window.draw(triangle);

                    sf::Color playerColor = p.isCT ? sf::Color::Cyan : sf::Color::Yellow;
                    
                    if (currenltyObserverdIsLower != p.isOnLowerLevel && !activeConfig.drawTwoMaps) {
                        playerColor -= sf::Color(0, 0, 0, 100);
                        triangle.setFillColor(sf::Color(255, 255, 255, 100));
                    }
                    else {
                        triangle.setFillColor(sf::Color::White);
                    }

                    playerCircle.setFillColor(playerColor);
                    window.draw(playerCircle);

                    // Draw the player's observer slot over the circle
                    if (p.observerSlot != -1) {
                        if (p.currentlyObserved) {
                            // Draw a white circle outline if the player is currenlty observed
                            playerCircle.setOutlineColor(sf::Color::White);
                            window.draw(playerCircle);
                            playerCircle.setOutlineColor(sf::Color::Black);
                        }
                        sf::Text& obsText = observerSlotTexts[p.observerSlot];

                        obsText.setCharacterSize(activeConfig.observerTextSize);
                        obsText.setPosition(p.minimapPosition);
                        if (p.isOnLowerLevel && activeConfig.drawTwoMaps) {
                            obsText.move(1024, 0);
                        }
                       
                        obsText.move(-1, -4);
                        window.draw(obsText);
                    }
                }
                else {
                    // Draw a cross if he is dead
                    crossSprite.setPosition(p.minimapPosition);

                    if (p.isOnLowerLevel && activeConfig.drawTwoMaps)
                        crossSprite.move(1024, 0);

                    crossSprite.move(-5, -5);
                    window.draw(crossSprite);
                }

                // Draw the player name
                p.playerNameText.setPosition(p.minimapPosition);
                if (p.isOnLowerLevel && activeConfig.drawTwoMaps)
                    p.playerNameText.move(1024, 0);

                p.playerNameText.move(-8, -2);

                p.playerNameText.setCharacterSize(p.dead ? activeConfig.nameDeadCharacterSize : activeConfig.nameCharacterSize);

                if (activeConfig.drawName)
                    window.draw(p.playerNameText);
            }
        }

        // Get the current connection status to CSGO and display it
        sf::Text status;
        status.setString(std::string("CSGO Status: ") + (gs.is_null() ? "disconnected" : "connected"));
        status.setPosition(0, 0);
        status.setFont(observerSlotFont);
        status.setFillColor(sf::Color::White);
        //window.draw(status);

        // Change the current map if it has changed in the game
        if (!gs.is_null() && !gs["map"].is_null() && !gs["map"]["name"].is_null()) {
            if (loadedMap == nullptr || loadedMap->map->name != gs["map"]["name"].get<std::string>()) {
                if (loadedMap != nullptr) delete loadedMap;
                loadedMap = loadMap(gs["map"]["name"].get<std::string>(), maps, 7, window, activeConfig);
                interp.currentlyLoadedMap = loadedMap;
            }
        }

        if (drawImGUI) {
            ImGui::Begin("Settings Window");

            ImGui::Text("Hide this window by pressing T");

            bool reloadSettings = false;

            if (ImGui::Button("Set Minimap Mode")) {
                activeConfig = configs[MINIMAP];
                reloadSettings = true;
            }

            if (ImGui::Button("Set Observer Mode")) {
                activeConfig = configs[OBSERVER];
                reloadSettings = true;
            }

            ImGui::Checkbox("Enable Interpolation", &enableInterpolation);
            ImGui::Text("Time since last Gamestate: %d ms", gamestate->timeSinceLastGamestate().asMilliseconds());

            if (reloadSettings) {
                if (loadedMap != nullptr) {
                    loadedMap = loadMap(gs["map"]["name"].get<std::string>(), maps, 7, window, activeConfig);
                    playerCircle.setRadius(activeConfig.circleSize);
                    playerCircle.setOrigin(activeConfig.circleSize, activeConfig.circleSize);
                    triangle.setRadius(activeConfig.circleSize);
                    triangle.setOrigin(activeConfig.circleSize, activeConfig.circleSize * 2);
                }
            }

            ImGui::End();
        }

        imgui_sfml_end_frame(window);

        window.display();
    }

    imgui_sfml_destroy();

    return EXIT_SUCCESS;
}