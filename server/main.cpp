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

/* Begin Event Senders */

void sendUpdatePlayerNumberEvent(int clientID, int player) // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
	server.wsSend(clientID, "{\"event\": \"updatePlayerNumberEvent\", \"player\": " + std::to_string(player) + "}");
	log("sendUpdatePlayerNumberEvent sent for client ID " + std::to_string(clientID) + " (player# = " + std::to_string(player) + ").");
}

void sendPlayerDisconnectEvent(int clientID, int player) // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
	server.wsSend(clientID, "{\"event\": \"playerDisconnectEvent\", \"player\": " + std::to_string(player) + "}");
	log("playerDisconnectEvent sent for client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " disconnected.)");
}

void sendPlayerConnectedEvent(int clientID, int player, std::string id)
{
	server.wsSend(clientID, "{\"event\": \"playerConnectedEvent\", \"id\":\"" + id + "\", \"player\": " + std::to_string(player) + "}");
	log("playerConnectedEvent sent for client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " connected.)");
}

void sendGameStartedEvent(int clientID)
{
	server.wsSend(clientID, "The game has been started!");
	server.wsSend(clientID, "{\"event\": \"gameStartedEvent\"}");
	log("gameStartedEvent sent for client ID " + std::to_string(clientID) + ")");
}

void sendPlayerDirectionEvent(int clientID, int player, std::string direction)
{
	server.wsSend(clientID, "{\"event\": \"playerDirectionEvent\", \"direction\":\"" + direction + "\", \"player\": " + std::to_string(player) + "}");
	log("playerDirectionEvent sent to client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " moved " + direction + ")");
}

void sendNewRewardEvent(int clientID, int X, int Y)
{
	server.wsSend(clientID, "{\"event\": \"newRewardEvent\", \"X\": " + std::to_string(X) + ", \"Y\": " + std::to_string(Y) + "}");
	log("newRewardEvent sent to client ID " + std::to_string(clientID) + " (X: " + std::to_string(X) + ", Y: " + std::to_string(Y) + ").");
}

void sendPlayerScoreRelayEvent(int clientID, int player, int index)
{
	server.wsSend(clientID, "{\"event\": \"playerScoreRelayEvent\", \"player\": " + std::to_string(player) + ", \"index\": " + std::to_string(index) + "}");
	log("playerScoreRelayEvent sent to client ID " + std::to_string(clientID) + " (Player: " + std::to_string(player) + ", index: " + std::to_string(index) + ").");
}
/* Begin Event Handlers */

void setPlayerIDEventHandler(int clientID, int size, std::string id)
{
	if (size <= 2)
	{
		if (!gameState.getPlayer1Online())
		{
			log("Player 1 has connected.");
			std::string previousID = gameState.getPlayerID(1);
			gameState.setPlayerID(1, id);
			//server.wsSend(clientID, previousID + "'s ID is now set to " + id);
			new_event("New player ID set for " + previousID + ": " + id);
			sendUpdatePlayerNumberEvent(clientID, 1);
		}
		else
		{
			log("Player 2 has connected.");
			std::string previousID = gameState.getPlayerID(2);
			gameState.setPlayerID(2, id);
			//server.wsSend(clientID, previousID + "'s ID is now set to " + id);
			new_event("New player ID set for " + previousID + ": " + id);
			sendUpdatePlayerNumberEvent(clientID, 2);
			sendPlayerConnectedEvent(1, PLAYER_1, gameState.getPlayerID(PLAYER_1)); // Tell Client ID 1 (Player 2) the information for Player 1
			sendPlayerConnectedEvent(0, PLAYER_2, gameState.getPlayerID(PLAYER_2)); // Tell Client ID 0 (Player 1) the information for Player 2
		}
	}

	log("setPlayerIDEventHandler fired.");


}

void setPlayerDirectionEventHandler(int clientID, int player, std::string direction)
{
	gameState.setPlayerDirection(player, direction);
	if (player == PLAYER_1) 
	{
		sendPlayerDirectionEvent(0, PLAYER_1, direction);
		sendPlayerDirectionEvent(1, PLAYER_1, direction);
	}
	else 
	{ 
		sendPlayerDirectionEvent(0, PLAYER_2, direction);
		sendPlayerDirectionEvent(1, PLAYER_2, direction);
	}
}

void playerScoreEventHandler(int clientID, int player, int index)
{
	log("playerScoreEventHandler fired.");
	gameState.incrementScore(player);
	sendPlayerScoreRelayEvent(0, player, index);
	sendPlayerScoreRelayEvent(1, player, index);
	//server.wsSend(clientID, gameState.getPlayerID(player) + " scored and now has " + std::to_string(gameState.getPlayerScore(player)) + " point(s).");
	//server.wsSend(clientID, gameState.getPlayerID(player) + " scored and now has " + std::to_string(gameState.getPlayerScore(player)) + " point(s).");
	new_event(gameState.getPlayerID(player) + " scored. New score: " + std::to_string(gameState.getPlayerScore(player)));
}

void setRewardEventHandler(int x, int y)
{
	log("setRewardEventHandler fired.");
	sendNewRewardEvent(0, x, y);
	sendNewRewardEvent(1, x, y);
}

void gameStartEventHandler()
{
	log("gameStartEventHandler fired.");
	gameState.resetScores();
	new_event("Player scores have been set back to 0 (client started game.)");
	sendGameStartedEvent(0);
	sendGameStartedEvent(1);
}

void gameFinishedEventHandler(int clientID) // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
	log("gameFinishedEventHandler fired.");
	server.wsSend(clientID, "Final Scores - (" + gameState.getPlayerID(PLAYER_1) + ": " + std::to_string(gameState.getPlayerScore(PLAYER_1)) + ", "
		+ gameState.getPlayerID(PLAYER_2) + ": " + std::to_string(gameState.getPlayerScore(PLAYER_2)) + ").");
	server.wsSend(clientID, "Scores have been reset back to 0.");
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
			sendPlayerDisconnectEvent(1, PLAYER_1); // Tell Client ID 1 (Player 2) that Player 1 has disconnected.
			server.wsClose(1);
		}
		gameState.setPlayer1Online(false);
		gameState.resetScores();
		log(gameState.getPlayerID(PLAYER_1) + " has disconnected (Player #1).");
		gameState.resetID(PLAYER_1);
	}
	else if (clientID == 1)
	{
		if (clientIDs.size() >= 2)
		{
			sendPlayerDisconnectEvent(0, PLAYER_2); // Tell Client ID 0 (Player 1) that Player 2 has disconnected.
		}
		gameState.resetScores();
		log(gameState.getPlayerID(PLAYER_2) + " has disconnected (Player #2)." + " Game state has been completely reset.");
		gameState.resetID(PLAYER_2);
	}
}

void messageHandler(int clientID, string message) 
{
	log("Message received by Client ID " + std::to_string(clientID) + ": " + message);

	std::string err; // This string is updated with an error message if the json parser fails.
	auto json = JSON::parse(message, err);
	std::string firedEvent = json["event"].string_value();
	std::vector<int> clientIDs = server.getClientIDs();

	// Begin Event Handling
	if (firedEvent == "setPlayerIDEvent") // JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
	{
		setPlayerIDEventHandler(clientID, clientIDs.size(), json["id"].string_value());
	}
	else if (firedEvent == "setPlayerDirectionEvent")
	{
		setPlayerDirectionEventHandler(clientID, json["player"].int_value(), json["direction"].string_value());
	}
	else if (firedEvent == "gameStartEvent") // JSON example -> {"event": "gameFinishedEvent"}
	{
		gameStartEventHandler();
	}
	else if (firedEvent == "playerScoreEvent") // JSON example -> {"event": "playerScoreEvent", "player": 1}
	{
		playerScoreEventHandler(clientID, json["player"].int_value(), json["index"].int_value());
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
