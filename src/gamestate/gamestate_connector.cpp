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
#include "gamestate_connector.h"

gamestate_connector::gamestate_connector(unsigned short port)
{
	server_thread = std::thread([this, port]() {
		server_start(port);
	});
}

gamestate_connector::~gamestate_connector()
{
	server.stop();
	server_thread.join();
}

sf::Time gamestate_connector::get_gamestate(gamestate* currentGamestate, gamestate* prevGamestate)
{
	gamestateMutex.lock();

	*currentGamestate = this->currentGamestate;
	*prevGamestate = this->prevGamestate;
	sf::Time returnValue = timeSinceLastUpdate;

	gamestateMutex.unlock();

	return returnValue;
}

bool gamestate_connector::gamestate_updated(bool reset)
{
	bool returnValue = gamestateUpdated;

	if (reset) {
		gamestateUpdated = false;
	}

	return returnValue;
}

void gamestate_connector::server_callback(const httplib::Request& req, httplib::Response& res)
{
	nlohmann::json rawGamestate = nlohmann::json::parse(req.body);
	sf::Time deltaTime = timeSinceLastUpdate.getElapsedTime();
	timeSinceLastUpdate.restart();

	res.status = 200;

	processIncomingGamestate();
}

void gamestate_connector::server_start(unsigned short port)
{
	server.Post("/", [this](const httplib::Request& req, httplib::Response& res) {
		server_callback(req, res);
	});

	server.listen("0.0.0.0", port);
}

void gamestate_connector::processIncomingGamestate(sf::Time deltaTime, const nlohmann::json& rawGamestate)
{
	gamestateMutex.lock();

	gamestateMutex.unlock();
}
