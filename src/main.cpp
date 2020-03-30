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
    mapinfo* selectedMap = nullptr;
    for (int i = 0; i < mapCount; i++) {
        if (maps[i].name == map) {
            selectedMap = maps + i;
        }
    }

    if (selectedMap == nullptr) {
        return nullptr;
    }

    loadedMap* returnMap = new loadedMap();

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
    int observerSlot;
    bool isCT;
    bool dead;
    float rotation;

    sf::Text playerNameString;

    player(nlohmann::json playerJson, sf::Font& playerFont) {
        std::string positionString = playerJson["position"];
        sscanf_s(positionString.c_str(), "%f, %f, %f", &position.x, &position.y, &position.z);

        if (!playerJson["observer_slot"].is_null()) {
            observerSlot = playerJson["observer_slot"].get<int>();
        }
        else {
            observerSlot = -1;
        }

        std::string team = playerJson["team"];
        if (team == "T") {
            isCT = false;
        }
        else {
            isCT = true;
        }

        if (playerJson["state"]["health"].get<int>() == 0) {
            dead = true;
        } else {
            dead = false;
        }

        playerNameString.setString(playerJson["name"].get<std::string>());
        playerNameString.setFillColor(isCT ? sf::Color::Cyan : sf::Color::Yellow);
        playerNameString.setOutlineColor(sf::Color::Black);
        playerNameString.setOutlineThickness(1);
        playerNameString.setCharacterSize(15); 
        playerNameString.setFont(playerFont);
        sf::FloatRect localBounds = playerNameString.getLocalBounds();
        playerNameString.setOrigin(floor(playerNameString.getLocalBounds().width), floor(playerNameString.getLocalBounds().height));


        std::string forwardString = playerJson["forward"];
        sf::Vector3f forward;
        sscanf_s(forwardString.c_str(), "%f, %f, %f", &forward.x, &forward.y, &forward.z);

        // Calculate rotation from 3D forward vector
        // There is probably a more clever way to do this but I came up with it and it works.
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
        std::cout << forwardString << " " << rotation << std::endl;
    }
};

int main()
{
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

    sf::CircleShape triangle(10, 3);
    triangle.setFillColor(sf::Color::White);
    triangle.setOutlineColor(sf::Color::Black);
    triangle.setOutlineThickness(1);
    triangle.setOrigin(10, 25);

    sf::Font observerSlotFont;
    observerSlotFont.loadFromFile("Roboto-Regular.ttf");
    sf::Font playerNameFont;
    playerNameFont.loadFromFile("Roboto-Regular.ttf");

    sf::Text observerSlotTexts[10];
    for (int i = 0; i < 10; i++) {
        observerSlotTexts[i].setCharacterSize(13);
        observerSlotTexts[i].setStyle(sf::Text::Bold);
        observerSlotTexts[i].setString(std::to_string(i));
        observerSlotTexts[i].setColor(sf::Color::Black);
        observerSlotTexts[i].setFont(observerSlotFont);
        observerSlotTexts[i].setOrigin(floor(observerSlotTexts[i].getLocalBounds().width / 2), floor(observerSlotTexts[i].getLocalBounds().height / 2));
    }

    sf::CircleShape playerCircle;
    playerCircle.setFillColor(sf::Color::Cyan);
    playerCircle.setOutlineColor(sf::Color::Black);
    playerCircle.setOutlineThickness(2);
    playerCircle.setRadius(10);
    playerCircle.setOrigin(10, 10);

    imgui_sfml_init("Roboto-Regular.ttf");

    csgo_gamestate* gamestate = new csgo_gamestate(1338);

    loadedMap* loadedMap = nullptr;

    nlohmann::json gs = gamestate->get_latest_gamestate();
    if (!gs.is_null() && !gs["map"].is_null() && !gs["map"]["name"].is_null()) {
         loadedMap = loadMap(gs["map"]["name"].get<std::string>(), maps, 7, window);
    }

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

        gs = gamestate->get_latest_gamestate();

        sf::Text status;
        status.setString(std::string("CSGO Status: ") + (gs.is_null() ? "disconnected" : "connected"));
        status.setPosition(0, 0);
        status.setFont(observerSlotFont);
        status.setFillColor(sf::Color::White);
        status.setColor(sf::Color::White);

        if (loadedMap != nullptr) {
            window.draw(loadedMap->mapSprite);

            if (loadedMap->map->hasTwoLayers) {
                window.draw(loadedMap->mapSpriteLower);
            }

            std::vector<player> players;
            for (auto p : gs["allplayers"].items()) {
                players.push_back(player(p.value(), playerNameFont));
            }

            std::sort(players.begin(), players.end(), [](player a, player b) { return a.position.z < b.position.z; });

            for (auto p : players) {
                if (!p.isCT) {
                    playerCircle.setFillColor(sf::Color::Yellow);
                }
                else {
                    playerCircle.setFillColor(sf::Color::Cyan);
                }

                sf::Vector3f minimapPosition3D = loadedMap->map->upperLeft - p.position;
                sf::Vector2f minimapPosition = sf::Vector2f((-minimapPosition3D.x / loadedMap->map->scale), (minimapPosition3D.y / loadedMap->map->scale));

                if (loadedMap->map->hasTwoLayers && minimapPosition3D.z > loadedMap->map->cutoff) {
                    minimapPosition.x += 1024;
                }

                playerCircle.setPosition(minimapPosition);
                p.playerNameString.setPosition(sf::Vector2f(minimapPosition.x - 4, minimapPosition.y - 2));

                window.draw(playerCircle);

                triangle.setRotation(p.rotation);
                triangle.setPosition(minimapPosition);
                window.draw(triangle);

                window.draw(p.playerNameString);

                if (p.observerSlot != -1) {
                    if (!gs["player"].is_null() && !gs["player"]["observer_slot"].is_null() &&  p.observerSlot == gs["player"]["observer_slot"].get<int>()) {
                        playerCircle.setOutlineColor(sf::Color::White);
                        //window.draw(playerCircle);
                        playerCircle.setOutlineColor(sf::Color::Black);
                    }
                    sf::Text& obsText = observerSlotTexts[p.observerSlot];
                    obsText.setPosition(sf::Vector2f(minimapPosition.x - 1, minimapPosition.y - 4)); 
                    window.draw(obsText);
                }

                if (p.dead) {
                    crossSprite.setPosition(minimapPosition);
                    window.draw(crossSprite);
                }
            }
        }

        window.draw(status);

        imgui_sfml_end_frame(window);

        // Update the window
        window.display();

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