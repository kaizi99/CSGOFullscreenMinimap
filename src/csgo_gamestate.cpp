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

#include "csgo_gamestate.h"
#include <iostream>
#include <mutex>

csgo_gamestate::csgo_gamestate(unsigned short port)
{
    server_thread = std::thread([this, port]() {
        server_start(port);
    });
}

csgo_gamestate::~csgo_gamestate() {
    server.stop();
    server_thread.join();
}

void csgo_gamestate::server_callback(const httplib::Request &req, httplib::Response &res) {
    gamestateMutex.lock();
    lastGamestate = nlohmann::json::parse(req.body);
    gamestateMutex.unlock();
    res.status = 200;
}

void csgo_gamestate::server_start(unsigned short port) {
    server.Post("/", [this](const httplib::Request& req, httplib::Response& res) {
        server_callback(req, res);
    });

    server.listen("0.0.0.0", port);
}

nlohmann::json csgo_gamestate::get_latest_gamestate() {
    std::lock_guard<std::mutex> lock(gamestateMutex);
    return lastGamestate;
}