/*
	ICS 167 | Group 2
	Project Server | main.cpp
*/

#include "websocket.h" // Library authored by the TA.
#include "json11.hpp" // Open-source C++ JSON parser library by MIT at https://github.com/dropbox/json11
#include "gamestate.h" // Class created to keep track of game state.

#include <iostream>
#include <vector>
#include <string>

typedef json11::Json JSON;

/* Begin Variables */

webSocket server;
GameState gameState;

/* Begin Helpers */

void log(string message)
{
	std::cout << "[LOG] " << message << std::endl;
}

void new_event(string message) // helps distinguish game state changes from log messages
{
	std::cout << "[EVENT] " << message << std::endl;
}

/* Begin Event Handlers */

void setPlayerIDEventHandler(int clientID, int player, std::string id)
{
	log("setPlayerIDEventHandler fired.");
	std::string previousID = gameState.getPlayerID(player);
	gameState.setPlayerID(player, id);
	server.wsSend(clientID, previousID + "'s ID is now set to " + id);
	new_event("New player ID set for " + previousID + ": " + id);
}

void setPlayerDirectionEventHandler(int clientID, int player, std::string direction)
{
	gameState.setPlayerDirection(player, direction);
	if (player == 1) 
	{
		server.wsSend(0, "P1 Direction: " + direction);
		server.wsSend(1, "P1 Direction: " + direction);
	}
	else { 
		server.wsSend(0, "P2 Direction: " + direction);
		server.wsSend(1, "P2 Direction: " + direction);
	}
}

void playerScoreEventHandler(int clientID, int player)
{
	log("playerScoreEventHandler fired.");
	gameState.incrementScore(player);
	server.wsSend(clientID, gameState.getPlayerID(player) + " scored and now has " + std::to_string(gameState.getPlayerScore(player)) + " point(s).");
	new_event(gameState.getPlayerID(player) + " scored. New score: " + std::to_string(gameState.getPlayerScore(player)));
}

void setRewardEventHandler(int x, int y)
{
	log("setRewardEventHandler fired.");
	server.wsSend(0, "RewardCoordinates-" + std::to_string(x) + "-"+std::to_string(y));
	server.wsSend(1, "RewardCoordinates-" + std::to_string(x) + "-" + std::to_string(y));
}

void gameStartEventHandler(int clientID)
{
	log("gameStartEventHandler fired.");
	server.wsSend(clientID, "Game has been started!");
	gameState.resetScores();
	server.wsSend(clientID, "Scores have been set to 0.");
	new_event("Player scores have been set back to 0 (client started game.)");
}

void gameFinishedEventHandler(int clientID) // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
	log("gameFinishedEventHandler fired.");
	server.wsSend(clientID, "Final Scores - (" + gameState.getPlayerID(PLAYER_1) + ": " + std::to_string(gameState.getPlayerScore(PLAYER_1)) + ", "
		+ gameState.getPlayerID(PLAYER_2) + ": " + std::to_string(gameState.getPlayerScore(PLAYER_2)) + ").");
	gameState.resetScores();
	server.wsSend(clientID, "Scores have been reset back to 0.");
	new_event("Player scores have been reset back to 0 (client finished game.)");
}

/* Begin Network Handlers */

void openHandler(int clientID)
{
	log("Connection opened. Client ID: " + std::to_string(clientID));
	std::vector<int> clientIDs = server.getClientIDs();

	if (clientIDs.size() > 2)
	{
		log("Rejecting connection..");
		server.wsSend(clientID, "There are already two clients connected. Rejecting your connection..");
		server.wsClose(clientID);
		log("Connection forcefully closed. (Reason: two clients already exist.) Client ID: " + std::to_string(clientID));
	} 
	else
	{
		log("Connection established.");
		server.wsSend(clientID, "Connection established.");
	}
}

void closeHandler(int clientID) 
{
	log("Connection closed. Client ID: " + std::to_string(clientID));
	std::vector<int> clientIDs = server.getClientIDs();
	server.wsSend(clientID, "Connection closed.");
	if (clientID == 0) // the main client
	{
		if (clientIDs.size() >= 2)
		{
			server.wsSend(1, "Player 1 Disconnected");//gameState.getPlayerID(PLAYER_1) + " has disconnected.");
			server.wsClose(1);
		}
		gameState.setPlayer1Online(false);
		gameState.reset();
		log(gameState.getPlayerID(PLAYER_1) + " has disconnected." + " Game state has been completely reset.");
	}
	else if (clientID == 1)
	{
		if (clientIDs.size() >= 2)
		{
			server.wsSend(0, "Player 2 Disconnected");//gameState.getPlayerID(PLAYER_2) + " has disconnected.");
		}
		gameState.reset();
		log(gameState.getPlayerID(PLAYER_2) + " has disconnected." + " Game state has been completely reset.");
	}
}

void messageHandler(int clientID, string message) 
{
	log("Message received by Client ID " + std::to_string(clientID) + ": " + message);

	std::string err; // This string is updated with an error message if the json parser fails.
	auto json = JSON::parse(message, err);
	std::string firedEvent = json["event"].string_value();

	// Begin Event Handling
	if (firedEvent == "setPlayerIDEvent") // JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
	{
		if (!gameState.getPlayer1Online())
		{
			log("player1 ");
			setPlayerIDEventHandler(clientID, 1, json["id"].string_value());
			server.wsSend(0, "Player#1");
		}
		else 
		{
			log("player2 ");
			setPlayerIDEventHandler(clientID, 2, json["id"].string_value());
			server.wsSend(1, "Player#2");
			server.wsSend(0, "Both clients have now been connected.");
			server.wsSend(0, "Ready to start game...");
			server.wsSend(1, "Ready to start game...");
		}
	}
	else if (firedEvent == "setPlayerDirectionEvent") // JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
	{
		setPlayerDirectionEventHandler(clientID, json["player"].int_value(), json["direction"].string_value());
	}
	else if (firedEvent == "gameStartEvent") // JSON example -> {"event": "gameFinishedEvent"}
	{
		gameStartEventHandler(0);
		gameStartEventHandler(1);
	}
	else if (firedEvent == "playerScoreEvent") // JSON example -> {"event": "playerScoreEvent", "player": 1}
	{
		playerScoreEventHandler(clientID, json["player"].int_value());
	}
	else if (firedEvent == "gameFinishedEvent") // JSON example -> {"event": "gameFinishedEvent"}
	{
		gameFinishedEventHandler(clientID);
	}
	else if (firedEvent == "rewardCoordinatesEvent") // JSON example -> {"event": "gameFinishedEvent"}
	{
		setRewardEventHandler(json["x"].int_value(), json["y"].int_value());
	}
	else
	{
		log("Error (if any): " + err);
		server.wsSend(clientID, "Received unexpected message: " + message);
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
