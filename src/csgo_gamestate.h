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

#ifndef PAUSE_CONTENT_CSGO_GAMESTATE_H
#define PAUSE_CONTENT_CSGO_GAMESTATE_H

#include "httplib.h"
#include <thread>
#include <functional>
#include "json.hpp"

class csgo_gamestate {
public:
    csgo_gamestate(unsigned short port);
    ~csgo_gamestate();

    //remove copy-constructor and copy-assignment
    csgo_gamestate(const csgo_gamestate& other) = delete;
    csgo_gamestate& operator=(const csgo_gamestate& other) = delete;

    nlohmann::json get_latest_gamestate();

    void add_callback(std::function<void(nlohmann::json)> callback);
private:
    void server_callback(const httplib::Request& req, httplib::Response& res);

    std::thread server_thread;
    httplib::Server server;
    void server_start(unsigned short port);

    std::mutex gamestateMutex;
    nlohmann::json lastGamestate;

    std::vector<std::function<void(nlohmann::json)>> callbacks;
};


#endif //PAUSE_CONTENT_CSGO_GAMESTATE_H
