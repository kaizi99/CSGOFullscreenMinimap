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

#include "imgui_sfml.h"
#include "csgo_gamestate.h"
#include <iostream>
#include <algorithm>

#define M_PI 3.14159265358979323846
#define M_PI_2 	1.57079632679489661923
#include <cmath>

struct mapinfo {
    std::string name;
    std::string radarName;
    sf::Vector3f upperLeft;
    float scale;
    bool hasTwoLayers;
    std::string lowerLayerName;
    float cutoff;
};

struct loadedMap {
    mapinfo* map;
    sf::Texture mapTexture;
    sf::Texture mapTextureLower;
    sf::Sprite mapSprite;
    sf::Sprite mapSpriteLower;
};

loadedMap* loadMap(std::string map, mapinfo* maps, int mapCount, sf::RenderWindow& window) {
    // Search for map in array (yes I know there is a binary search but the dataset is really
    // small and unsorted so this is okay to do)
    mapinfo* selectedMap = nullptr;
    for (int i = 0; i < mapCount; i++) {
        if (maps[i].name == map) {
            selectedMap = maps + i;
        }
    }

    if (selectedMap == nullptr) {
        return nullptr;
    }

    // allocate new map info
    loadedMap* returnMap = new loadedMap();

    // load map info
    returnMap->map = selectedMap;
    returnMap->mapTexture.loadFromFile(selectedMap->radarName);
    returnMap->mapSprite = sf::Sprite(returnMap->mapTexture);

    if (selectedMap->hasTwoLayers) {
        window.setSize(sf::Vector2u(2048, 1024)); 
        window.setView(sf::View(sf::Vector2f(1024, 512), sf::Vector2f(2048, 1024)));
        returnMap->mapTextureLower.loadFromFile(selectedMap->lowerLayerName);
        returnMap->mapSpriteLower = sf::Sprite(returnMap->mapTextureLower);
        returnMap->mapSpriteLower.setPosition(sf::Vector2f(1024, 0));
    }
    else {
        window.setSize(sf::Vector2u(1024, 1024));
        window.setView(sf::View(sf::Vector2f(512, 512), sf::Vector2f(1024, 1024)));
    }

    return returnMap;
}

struct player {
    sf::Vector3f position;
    sf::Vector2f minimapPosition;
    bool isOnLowerLevel;
    int observerSlot;
    bool isCT;
    bool dead;
    float rotation;

    sf::Text playerNameText;

    player(nlohmann::json playerJson, sf::Font& playerFont, loadedMap* loadedMap) {
        // Determine player 3D position
        std::string positionString = playerJson["position"];
        sscanf(positionString.c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);

        // Calculate the position on the minimap based on the 3D position
        sf::Vector3f minimapPosition3D = loadedMap->map->upperLeft - position;
        minimapPosition = sf::Vector2f((-minimapPosition3D.x / loadedMap->map->scale), (minimapPosition3D.y / loadedMap->map->scale));

        if (loadedMap->map->hasTwoLayers && minimapPosition3D.z > loadedMap->map->cutoff) {
            isOnLowerLevel = true;
        }
        else {
            isOnLowerLevel = false;
        }

        // Determine observer slot
        if (!playerJson["observer_slot"].is_null()) {
            observerSlot = playerJson["observer_slot"].get<int>();
        }
        else {
            observerSlot = -1;
        }

        // Determine team
        std::string team = playerJson["team"];
        if (team == "T") {
            isCT = false;
        }
        else {
            isCT = true;
        }
        
        // Determine alive status
        if (playerJson["state"]["health"].get<int>() == 0) {
            dead = true;
        } else {
            dead = false;
        }

        // Setup player name text
        playerNameText.setString(playerJson["name"].get<std::string>());
        playerNameText.setFillColor(isCT ? sf::Color::Cyan : sf::Color::Yellow);
        playerNameText.setOutlineColor(sf::Color::Black);
        playerNameText.setOutlineThickness(1);
        playerNameText.setCharacterSize(dead ? 12 : 15); 
        playerNameText.setFont(playerFont);
        sf::FloatRect localBounds = playerNameText.getLocalBounds();
        playerNameText.setOrigin(floor(playerNameText.getLocalBounds().width), floor(playerNameText.getLocalBounds().height));

        // Calculate rotation from 3D forward vector
        // There is probably a more clever way to do this but I came up with it and it works.
        std::string forwardString = playerJson["forward"];
        sf::Vector3f forward;
        sscanf(forwardString.c_str(), "%f, %f, %f", &forward.x, &forward.y, &forward.z);

        float angle;
        if (forward.x > 0) {
            angle = acos(forward.y);
        }
        else if (forward.x < 0) {
            angle = asin(forward.y) + M_PI + M_PI_2;
        }
        else {
            if (forward.y > 0) {
                angle = 0;
            }
            else {
                angle = M_PI;
            }
        }

        rotation = (angle / (M_PI * 2)) * 360;
    }
};

int main()
{
    // Hardcoded maps, this will be replaced by an ImGui interface and saved json
    mapinfo maps[7];
    maps[0].name = "de_overpass";
    maps[0].radarName = "de_overpass_radar.png";
    maps[0].upperLeft = sf::Vector3f(-4831, 1781, 0);
    maps[0].scale = 5.2;
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
    maps[3].scale = 4.4;
    maps[3].hasTwoLayers = false;

    maps[4].name = "de_inferno";
    maps[4].radarName = "de_inferno_radar.png";
    maps[4].upperLeft = sf::Vector3f(-2087, 3870, 0);
    maps[4].scale = 4.9;
    maps[4].hasTwoLayers = false;

    maps[5].name = "de_mirage";
    maps[5].radarName = "de_mirage_radar.png";
    maps[5].upperLeft = sf::Vector3f(-3230, 1713, 0);
    maps[5].scale = 5;
    maps[5].hasTwoLayers = false;

    maps[6].name = "de_train";
    maps[6].radarName = "de_train_radar.png";
    maps[6].upperLeft = sf::Vector3f(-2477, 2392, 0);
    maps[6].scale = 4.7;
    maps[6].hasTwoLayers = false;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1024, 1024), "Fullscreen CSGO Map by kaizi99");
    
    // Setup the cross sprite
    sf::Texture cross;
    cross.loadFromFile("cross.png");
    sf::Sprite crossSprite(cross);
    crossSprite.setOrigin(5, 5);
    crossSprite.setScale(sf::Vector2f(0.75, 0.75));

    // Setup the direction triangle
    sf::CircleShape triangle(10, 3);
    triangle.setFillColor(sf::Color::White);
    triangle.setOutlineColor(sf::Color::Black);
    triangle.setOutlineThickness(1);
    triangle.setOrigin(10, 18);

    // Load the observer slot and player name fonts
    sf::Font observerSlotFont;
    observerSlotFont.loadFromFile("Roboto-Regular.ttf");
    sf::Font playerNameFont;
    playerNameFont.loadFromFile("Roboto-Regular.ttf");

    // Setup the observer slot texts
    sf::Text observerSlotTexts[10];
    for (int i = 0; i < 10; i++) {
        observerSlotTexts[i].setCharacterSize(15);
        observerSlotTexts[i].setStyle(sf::Text::Bold);
        observerSlotTexts[i].setString(std::to_string(i));
        observerSlotTexts[i].setColor(sf::Color::Black);
        observerSlotTexts[i].setFont(observerSlotFont);
        observerSlotTexts[i].setOrigin(floor(observerSlotTexts[i].getLocalBounds().width / 2), floor(observerSlotTexts[i].getLocalBounds().height / 2));
    }

    // Setup the player circle
    sf::CircleShape playerCircle;
    playerCircle.setFillColor(sf::Color::Cyan);
    playerCircle.setOutlineColor(sf::Color::Black);
    playerCircle.setOutlineThickness(2);
    playerCircle.setRadius(10);
    playerCircle.setOrigin(10, 10);

    // Initialize the ImGui and the SFML integration
    imgui_sfml_init("Roboto-Regular.ttf");

    // Start the csgo gamestate http server on port 1338
    csgo_gamestate* gamestate = new csgo_gamestate(1338);

    loadedMap* loadedMap = nullptr;

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

            imgui_sfml_process_event(event);
        }

        imgui_sfml_begin_frame(window, 0.1);

        // Clear screen
        window.clear(sf::Color(0, 0, 0, 255));

        auto gs = gamestate->get_latest_gamestate();

        // Only draw stuff if there is a supported map
        if (loadedMap != nullptr) {
            // Draw the overviews
            window.draw(loadedMap->mapSprite);

            if (loadedMap->map->hasTwoLayers) {
                window.draw(loadedMap->mapSpriteLower);
            }

            // Get all players from gamestate
            std::vector<player> players;
            for (auto p : gs["allplayers"].items()) {
                players.push_back(player(p.value(), playerNameFont, loadedMap));
            }

            // Sort all dead players under the alive players to draw the alive player always above dead players
            std::sort(players.begin(), players.end(), [](player a, player b) { if (a.dead && !b.dead) return true; else return false; });
            
            // Find the first alive player in the list
            std::vector<player>::iterator it;
            for (it = players.begin(); it != players.end(); ++it) {
                if (!it->dead) {
                    break;
                }
            }

            // Sort all alive players based on height in the map to draw boosted players over the boosters and dead players
            std::sort(it, players.end(), [](player a, player b) { return a.position.z < b.position.z; });

            for (auto p : players) {
                // Draw the player's rotation and player circle if he isn't dead
                if (!p.dead) {
                    triangle.setRotation(p.rotation);
                    triangle.setPosition(p.minimapPosition);
                    window.draw(triangle);

                    playerCircle.setPosition(!p.isOnLowerLevel ? p.minimapPosition : sf::Vector2f(p.minimapPosition.x + 1024, p.minimapPosition.y));
                    playerCircle.setFillColor(p.isCT ? sf::Color::Cyan : sf::Color::Yellow);
                    window.draw(playerCircle);
                }

                // Draw the player name
                p.playerNameText.setPosition(sf::Vector2f(p.minimapPosition.x - 8, p.minimapPosition.y - 2));
                window.draw(p.playerNameText);

                // Draw the player's observer slot over the circle
                if (p.observerSlot != -1) {
                    if (!gs["player"].is_null() && !gs["player"]["observer_slot"].is_null() &&  p.observerSlot == gs["player"]["observer_slot"].get<int>()) {
                        playerCircle.setOutlineColor(sf::Color::White);
                        window.draw(playerCircle);
                        playerCircle.setOutlineColor(sf::Color::Black);
                    }
                    sf::Text& obsText = observerSlotTexts[p.observerSlot];
                    obsText.setPosition(sf::Vector2f(p.minimapPosition.x - 1, p.minimapPosition.y - 4));
                    window.draw(obsText);
                }

                // Draw either a cross if the player is dead or the current player rotation
                if (p.dead) {
                    crossSprite.setPosition(p.minimapPosition.x - 5, p.minimapPosition.y - 5);
                    window.draw(crossSprite);
                }
            }
        }

        // Get the current connection status to CSGO and display it
        sf::Text status;
        status.setString(std::string("CSGO Status: ") + (gs.is_null() ? "disconnected" : "connected"));
        status.setPosition(0, 0);
        status.setFont(observerSlotFont);
        status.setFillColor(sf::Color::White);
        status.setColor(sf::Color::White);
        window.draw(status);

        imgui_sfml_end_frame(window);

        window.display();

        // Change the current map if it has changed in the game
        if (!gs.is_null() && !gs["map"].is_null() && !gs["map"]["name"].is_null()) {
            if (loadedMap == nullptr || loadedMap->map->name != gs["map"]["name"].get<std::string>()) {
                if (loadedMap != nullptr) delete loadedMap;
                loadedMap = loadMap(gs["map"]["name"].get<std::string>(), maps, 7, window);
            }
        }
    }

    imgui_sfml_destroy();

    return EXIT_SUCCESS;
}