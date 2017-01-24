/*
	ICS 167 | Group 2
	Project Server | main.cpp
	Author: Thomas T Nguyen
*/

#include "websocket.h" // Library authored by the TA.
#include "json11.hpp" // C++ JSON parser library by MIT at https://github.com/dropbox/json11
#include "gamestate.h" // Class created to keep track of game state.

#include <iostream>
#include <vector>
#include <string>
#include <map>

typedef json11::Json JSON;

/* Begin Variables */
webSocket server;
GameState gameState;

/* Helpers */
void log(string message)
{
	std::cout << "[LOG] " << message << std::endl;
}

/* called when a client connects */
void openHandler(int clientID)
{
	log("Connection opened. Client ID: " + std::to_string(clientID));
	std::vector<int> clientIDs = server.getClientIDs();

	if (clientIDs.size() >= 2)
	{
		server.wsSend(clientID, "There is already a machine connected. Rejecting your connection..");
		server.wsClose(clientID);
		log("Connection forcefully closed. (Reason: a connection already exists.) Client ID: " + std::to_string(clientID));
	} 
	else server.wsSend(clientID, "Connection established."); // TODO: implement ID and Score checking.
}

/* called when a client disconnects */
void closeHandler(int clientID) 
{
	log("Connection closed. Client ID: " + std::to_string(clientID));
	server.wsSend(clientID, "Connection closed.");
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message) 
{
	log("Message received by Client ID " + std::to_string(clientID) + ": " + message);

	std::string err; // This string is updated with an error message if the json parser fails.
	auto json = JSON::parse(message, err);
	std::string firedEvent = json["event"].string_value();

	log("Event received: " + firedEvent + " | Error (if any): " + err);

	// Begin Event Handling
	if (firedEvent == "setPlayerIDEvent") // JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
	{
		if (json["player"].int_value() == PLAYER_1) // note: we can move all of this to a function later
		{
			gameState.setPlayerID(PLAYER_1, json["id"].string_value());
			server.wsSend(clientID, "Player 1 ID set to " + json["id"].string_value());
		}
		else
		{
			gameState.setPlayerID(PLAYER_2, json["id"].string_value());
			server.wsSend(clientID, "Player 2 ID set to " + json["id"].string_value());
		}
		log("New player ID set for " + std::to_string(json["player"].int_value()) + ": " + json["id"].string_value());
	}
} 

/* Begin Main Function */
int main()
{
	int port;

	std::cout << "[INIT] Please enter the desired port: ";
	std::cin >> port;

	server.setOpenHandler(openHandler);
	server.setCloseHandler(closeHandler);
	server.setMessageHandler(messageHandler);

	server.startServer(port);

	return 0;
}