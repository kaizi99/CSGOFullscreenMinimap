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

#include "gamestate.h"
#include "httplib.h"

#include <SFML/System.hpp>
#include <thread>

class gamestate_connector {
public:
    gamestate_connector(unsigned short port);
    ~gamestate_connector();

    // since this is multithreaded we should not move or copy this
    gamestate_connector(const gamestate_connector& other) = delete;
    gamestate_connector& operator=(const gamestate_connector& other) = delete;
    gamestate_connector(const gamestate_connector&& other) = delete;
    gamestate_connector& operator=(const gamestate_connector&& other) = delete;

    sf::Time get_gamestate(gamestate* currentGamestate, gamestate* prevGamestate);
    bool gamestate_updated(bool reset = true);

private:
    void server_callback(const httplib::Request& req, httplib::Response& res);
    void server_start(unsigned short port);
    
    void processIncomingGamestate(sf::Time deltaTime, const nlohmann::json& rawGamestate);

    std::thread server_thread;
    httplib::Server server;

    sf::Clock timeSinceLastUpdate;

    std::mutex gamestateMutex;
    gamestate currentGamestate;
    gamestate prevGamestate;

    bool gamestateUpdated;
};