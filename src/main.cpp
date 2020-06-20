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
#include <fstream>
#include <memory>

#include "imgui_sfml.h"
#include "csgo_gamestate.h"
#include "player.h"
#include "map.h"
#include "interpolation.h"
#include "draw_config.h"
#include "bomb.h"
#include "grenades.h"
#include "config_editor.h"

#include <imgui_stdlib.h>

#ifdef _MSC_VER
#include <direct.h>
#endif

int main()
{
#ifdef _MSC_VER && _DEBUG
    chdir("../../../workdir/");
#endif

    std::ifstream configFile("config.json");

    if (!configFile.is_open()) {
        std::cout << "Could not open config" << std::endl;
        return -1;
    }

    nlohmann::json configJson = nlohmann::json::parse(configFile);
    configFile.close();

    std::vector<mapinfo> mapinfos = mapinfo_parse_json(configJson["maps"]);
    std::vector<draw_config> drawConfigs = draw_config_parse_json(configJson["configs"]);

    int gamestatePort = configJson["gamestatePort"].get<int>();

    config_editor confed(&mapinfos, &drawConfigs, gamestatePort);

    draw_config activeConfig = drawConfigs[0];

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1024, 1024), "Fullscreen CSGO Map by kaizi99");
    window.setFramerateLimit(60);
    
    // Setup the cross sprite
    sf::Texture cross;
    cross.loadFromFile("cross.png");
    sf::Sprite crossSprite(cross);
    crossSprite.setOrigin(10, 10);
    crossSprite.setScale(sf::Vector2f(0.75, 0.75));

    // Setup the bomb sprite
    sf::Texture bombTexture;
    bombTexture.loadFromFile("bomb.png");
    sf::Sprite bombSprite(bombTexture);
    bombSprite.setOrigin(10, 10);
    bombSprite.setScale(activeConfig.bombIconScale, activeConfig.bombIconScale);

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

    sf::Font bauchbindeFont;
    bauchbindeFont.loadFromFile("segoeui.ttf");

    // Load the grenade textures
    std::shared_ptr<grenadeResources> grenadeTextures = std::make_shared<grenadeResources>();
    grenadeTextures->decoy.loadFromFile("grenades/weapon_decoy.png");
    grenadeTextures->flashbang.loadFromFile("grenades/weapon_flashbang.png");
    grenadeTextures->hegrenade.loadFromFile("grenades/weapon_hegrenade.png"); 
    grenadeTextures->incgrenade.loadFromFile("grenades/weapon_incgrenade.png");
    grenadeTextures->molotov.loadFromFile("grenades/weapon_molotov.png");
    grenadeTextures->smokegrenade.loadFromFile("grenades/weapon_smokegrenade.png");

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
    imgui_sfml_init("Roboto-Regular.ttf", window.getSize().x, window.getSize().y);

    // Start the csgo gamestate http server on port 1338
    csgo_gamestate* gamestate = new csgo_gamestate(1338);
    interpolation interp(gamestate);

    loadedMap* loadedMap = nullptr;

    bool drawImGUI = true;
    bool enableInterpolation = true;

    std::string pickedByLogo = "depa.png";
    bool drawPickedBy = false;
    sf::Texture pickedByLogoTexture;

    sf::Clock deltaTimeClock;
    deltaTimeClock.restart();

    float deltaTime = 0.16;

    // Start the game loop
    while (window.isOpen())
    {
        try {
            static sf::Vector2f oldMousePosition = sf::Vector2f(0, 0);

            // Process events
            sf::Event event;
            while (window.pollEvent(event))
            {
                // Close window: exit
                if (event.type == sf::Event::Closed)
                    window.close();
                // Switch the Settings window
                else if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                    case sf::Keyboard::T:
                        drawImGUI = !drawImGUI;
                        break;
                    case sf::Keyboard::A:
                        if (loadedMap) {
                            window.setView(loadedMap->map.aSiteView.getSFMLView());
                        }
                        break;
                    case sf::Keyboard::S:
                        if (loadedMap) {
                            window.setView(loadedMap->map.standardView.getSFMLView());
                        }
                        break;
                    case sf::Keyboard::D:
                        if (loadedMap) {
                            window.setView(loadedMap->map.bSiteView.getSFMLView());
                        }
                        break;
                    }
                }
                // Handle resize properly
                else if (event.type == sf::Event::Resized) {
                    sf::View v = window.getView();
                    v.setSize(event.size.width, event.size.height);
                    v.setCenter(event.size.width / 2, event.size.height / 2);
                    window.setView(v);
                }
                // Adjust view with scrolling
                else if (event.type == sf::Event::MouseWheelScrolled) {
                    sf::View v = window.getView();
                    sf::Vector2f size = v.getSize();

                    size += size * event.mouseWheelScroll.delta * 0.1f;

                    v.setSize(size);
                    window.setView(v);
                }

                imgui_sfml_process_event(event);
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && window.hasFocus()) {
                sf::View v = window.getView();
                sf::Vector2f center = v.getCenter();

                center += (oldMousePosition - sf::Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y));

                v.setCenter(center);
                window.setView(v);
            }

            oldMousePosition = sf::Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);

            imgui_sfml_begin_frame(window, deltaTime);

            // Clear screen
            window.clear(sf::Color(0, 0, 0, 150));

            auto gs = gamestate->get_latest_gamestate();

            std::vector<player> players;

            // Only draw stuff if there is a supported map
            if (loadedMap != nullptr) {

                bomb b(gs["bomb"], loadedMap);

                if (enableInterpolation) {
                    // Get all players interpolated
                    players = interp.processInterpolation();
                }
                else {
                    for (auto p : gs["allplayers"].items()) {
                        players.push_back(player(p.value(), p.key(), gs["player"], observerSlotFont, loadedMap));
                    }
                }

                auto grenades = processGrenades(gs, grenadeTextures, players);

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
                if (loadedMap->map.hasTwoLayers) {
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

                // Draw the grenades
                for (const auto& g : grenades) {
                    g.second->render(window, loadedMap->map, activeConfig);
                }

                // Draw each player
                for (auto p : players) {
                    if (!p.dead) {
                        // Draw the player's rotation and player circle if he isn't dead
                        triangle.setRotation(p.rotation);

                        if (p.isOnLowerLevel && activeConfig.drawTwoMaps) {
                            playerCircle.setPosition(p.minimapPosition + loadedMap->map.lowerLayerOffset);
                            triangle.setPosition(p.minimapPosition + loadedMap->map.lowerLayerOffset);
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
                            obsText.setOrigin(obsText.getLocalBounds().width / 2.0f, obsText.getLocalBounds().height / 2.0f);
                            obsText.setPosition(p.minimapPosition.x, p.minimapPosition.y - (activeConfig.circleSize / 4));

                            if (p.isOnLowerLevel && activeConfig.drawTwoMaps) {
                                obsText.move(loadedMap->map.lowerLayerOffset);
                            }

                            window.draw(obsText);
                        }
                    }
                    else {
                        // Draw a cross if he is dead
                        crossSprite.setPosition(p.minimapPosition);

                        if (p.isOnLowerLevel && activeConfig.drawTwoMaps)
                            crossSprite.move(loadedMap->map.lowerLayerOffset);

                        window.draw(crossSprite);
                    }

                    // Draw the player name
                    p.playerNameText.setPosition(p.minimapPosition);
                    if (p.isOnLowerLevel && activeConfig.drawTwoMaps)
                        p.playerNameText.move(loadedMap->map.lowerLayerOffset);

                    p.playerNameText.move(-8, -2);
                    p.playerNameText.setCharacterSize(p.dead ? activeConfig.nameDeadCharacterSize : activeConfig.nameCharacterSize);

                    if (activeConfig.drawName)
                        window.draw(p.playerNameText);
                }

                // Draw the bomb
                if (b.state != bomb_state::UNDEFINED) {
                    sf::Vector2f bPos = b.minimapPosition;

                    if (b.state == bomb_state::CARRIED) {
                        for (const auto& p : players) {
                            if (p.steamID == b.carrierID) {
                                bPos = p.minimapPosition;
                                bPos.x += playerCircle.getRadius();
                                bPos.y -= playerCircle.getRadius();
                            }
                        }
                    }

                    if (b.isOnLowerLevel && activeConfig.drawTwoMaps) {
                        bombSprite.setPosition(bPos + loadedMap->map.lowerLayerOffset);
                    }
                    else {
                        bombSprite.setPosition(bPos);
                    }

                    switch (b.state) {
                    case bomb_state::CARRIED:
                        bombSprite.setColor(sf::Color::Yellow);
                        break;
                    case bomb_state::DROPPED:
                        bombSprite.setColor(sf::Color(255, 100, 33, 255));
                        break;
                    case bomb_state::PLANTED:
                        bombSprite.setColor(sf::Color::Red);
                        break;
                    case bomb_state::DEFUSED:
                        bombSprite.setColor(sf::Color::Green);
                        break;
                    }

                    window.draw(bombSprite);
                }

                // Draw the footer
                if (activeConfig.drawBauchbinde) {
                    sf::View oldView = window.getView();

                    sf::View igview(sf::FloatRect({ 0, 0 }, { (float)window.getSize().x, (float)window.getSize().y }));
                    window.setView(igview);

                    sf::RectangleShape bauchbindeShape;
                    bauchbindeShape.setSize(sf::Vector2f(1024, 200));
                    bauchbindeShape.setPosition(sf::Vector2f(0, 920));
                    bauchbindeShape.setFillColor(sf::Color(50, 50, 50, 100));
                    window.draw(bauchbindeShape);

                    if (drawPickedBy) {
                        sf::Text bauchbindeText(loadedMap->map.name + " picked by ", bauchbindeFont);
                        bauchbindeText.setCharacterSize(60);
                        float textHeight = bauchbindeText.getLocalBounds().height;
                        float textWidth = bauchbindeText.getLocalBounds().width;

                        float bauchbindeContentWidth = bauchbindeText.getLocalBounds().width + 10 + textHeight;
                        bauchbindeText.setOrigin(bauchbindeText.getLocalBounds().width / 2, bauchbindeText.getLocalBounds().height / 2);
                        bauchbindeText.setPosition(512 - 10 - textHeight, 960);

                        // The *1.5 is a really bad hack to fix something I currenlty dont have the time to fix
                        sf::Sprite pickedByLogoSprite(pickedByLogoTexture);
                        pickedByLogoSprite.setScale((textHeight / pickedByLogoTexture.getSize().x) * 1.5, (textHeight / pickedByLogoTexture.getSize().y) * 1.5);
                        pickedByLogoSprite.setOrigin(0, pickedByLogoTexture.getSize().y / 2);
                        pickedByLogoSprite.setPosition(512 - textHeight + (textWidth / 2), 975);

                        window.draw(bauchbindeText);
                        window.draw(pickedByLogoSprite);
                    } else {
                        sf::Text bauchbindeText(loadedMap->map.name, bauchbindeFont);
                        bauchbindeText.setCharacterSize(60);

                        bauchbindeText.setOrigin(bauchbindeText.getLocalBounds().width / 2, bauchbindeText.getLocalBounds().height / 2);
                        bauchbindeText.setPosition(512, 960);

                        window.draw(bauchbindeText);
                    }

                    window.setView(oldView);
                }
            }

            // Change the current map if it has changed in the game
            if (!gs.is_null() && !gs["map"].is_null() && !gs["map"]["name"].is_null()) {
                if (loadedMap == nullptr || loadedMap->map.name != gs["map"]["name"].get<std::string>()) {
                    if (loadedMap != nullptr) delete loadedMap;
                    loadedMap = loadMap(gs["map"]["name"].get<std::string>(), mapinfos, window, activeConfig);
                    interp.currentlyLoadedMap = loadedMap;
                    window.setView(loadedMap->map.standardView.getSFMLView());
                }
            }

            if (drawImGUI) {
                ImGui::Begin("Settings Window");

                ImGui::Text("Hide this window by pressing T");

                bool reloadSettings = false;

                if (ImGui::Button("Set Minimap Mode")) {
                    activeConfig = drawConfigs.at(0);
                    reloadSettings = true;
                }

                if (ImGui::Button("Set Observer Mode")) {
                    activeConfig = drawConfigs.at(1);
                    reloadSettings = true;
                }

                ImGui::Checkbox("Enable Interpolation", &enableInterpolation);
                ImGui::Text("Time since last Gamestate: %d ms", gamestate->timeSinceLastGamestate().asMilliseconds());

                if (reloadSettings) {
                    if (loadedMap != nullptr) {
                        loadedMap = loadMap(gs["map"]["name"].get<std::string>(), mapinfos, window, activeConfig);
                        playerCircle.setRadius(activeConfig.circleSize);
                        playerCircle.setOrigin(activeConfig.circleSize, activeConfig.circleSize);
                        triangle.setRadius(activeConfig.circleSize);
                        triangle.setOrigin(activeConfig.circleSize, activeConfig.circleSize * 2);
                        bombSprite.setScale(activeConfig.bombIconScale, activeConfig.bombIconScale);
                    }
                }

                if (ImGui::Button("Open Config Editor")) {
                    confed.showWindow();
                }

                if (ImGui::Button("Copy gamestate clipboard")) {
                    sf::Clipboard::setString(gamestate->get_latest_gamestate().dump());
                }

            	if (ImGui::Button("Copy SteamIDs to clipboard"))
            	{
                    std::string clipboardString;
                    for (player p : players)
                    {
                        clipboardString += p.steamID + " - ";
                        clipboardString += p.playerNameText.getString().toAnsiString() + "\n";
					}

                    sf::Clipboard::setString(clipboardString);
            	}

                ImGui::InputText("Logo Path", &pickedByLogo);
                if (ImGui::Button("Load Logo")) {
                    pickedByLogoTexture.loadFromFile(pickedByLogo);
                    pickedByLogoTexture.generateMipmap();
                }

                ImGui::Checkbox("Draw picked by", &drawPickedBy);
                
                if (ImGui::Button("Encode View to clipboard")) {
                    std::string clipboardString;
                    
                    sf::Vector2f center = window.getView().getCenter();
                    sf::Vector2f size = window.getView().getSize();

                    view v;
                    v.width = size.x;
                    v.height = size.y;
                    v.centerX = center.x;
                    v.centerY = center.y;

                    clipboardString = encodeView(v).dump(4);

                    sf::Clipboard::setString(clipboardString);
                }

                ImGui::End();
            }

            confed.drawSettingsWindow(window);

            imgui_sfml_end_frame(window);

            window.display();
        } catch (nlohmann::json::exception e)
        {
            std::cerr << "A JSON exception occured. If this doesn't happen more than a couple times an hour you can ignore this." << std::endl;
            std::cerr << "JSON exceptions happen when CSGO sends information with certain critical information either missing or malformed." << std::endl;

            std::cerr << e.what() << std::endl;
        }

        deltaTime = deltaTimeClock.getElapsedTime().asMilliseconds() / 1000.0f;
        deltaTimeClock.restart();
    }

    imgui_sfml_destroy();

    return EXIT_SUCCESS;
}
