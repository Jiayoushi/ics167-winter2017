/*
	ICS 167 | Group 2
	Project Server | main.cpp
*/

#include "websocket.h" // Library authored by the TA.
#include "json11.hpp" // Open-source C++ JSON parser library by MIT at https://github.com/dropbox/json11
#include "gamestate.h" // Class created to keep track of game state.
#include "gamelogic.h"

#include <iostream>
#include <vector>
#include <string>

typedef json11::Json JSON;

/* Begin Variables */

webSocket server;
GameState gameState;
GameLogic gameLogic;

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

void sendLoopEvent(int clientID)
{
    server.wsSend(clientID, "{\"event\": \"loopEvent\"}");
}


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

void sendNewRewardEvent(int clientID, int X, int Y, int index)
{
	server.wsSend(clientID, "{\"event\": \"newRewardEvent\", \"X\": " + std::to_string(X) + ", \"Y\": " + std::to_string(Y) + ", \"index\": " + std::to_string(index) + "}");
	log("newRewardEvent sent to client ID " + std::to_string(clientID) + " (X: " + std::to_string(X) + ", Y: " + std::to_string(Y) + ", index: "+ std::to_string(index) + ").");
}

void sendGameFinishedEvent(int clientID)
{
    std::string winner = gameLogic.getWinner();
    server.wsSend(clientID, "{\"event\": \"gameFinishedEvent\", \"winner\":\"" + winner + "\"}");
   
    log("gameFinishedEvent broadcast");
}


void sendPlayerScoreRelayEvent(int clientID, int player)
{
	server.wsSend(clientID, "{\"event\": \"playerScoreRelayEvent\", \"player\": " + std::to_string(player)  + "}");
	log("playerScoreRelayEvent sent to client ID " + std::to_string(clientID) + " (Player: " + std::to_string(player) + ").");
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
    log("PlayerDirectionEventHandler fired");
    gameLogic.setDirection(player,direction);
    
    sendPlayerDirectionEvent(0, player, direction);
    sendPlayerDirectionEvent(1, player, direction);
}


void loopEventHandler()
{
    sendLoopEvent(0);
    sendLoopEvent(1);
}

/* Broacast which player scores */
void playerScoreEventHandler(int player)
{
	log("playerScoreEventHandler fired.");
	
    gameLogic.incrementScore(player);

	sendPlayerScoreRelayEvent(0, player);
	sendPlayerScoreRelayEvent(1, player);
	
    new_event(gameState.getPlayerID(player) + " scored. New score: " + std::to_string(gameState.getPlayerScore(player)));
}

void RewardEventHandler(int x, int y, int index)
{
	log("RewardEventHandler fired.");
	sendNewRewardEvent(0, x, y, index);
	sendNewRewardEvent(1, x, y, index);
}

void gameStartEventHandler()
{

	log("gameStartEventHandler fired.");
	new_event("Player scores have been set back to 0 (client started game.)");

	sendGameStartedEvent(0);
	sendGameStartedEvent(1);

    // -1 indicates there is no reward to be removed.
    RewardEventHandler(gameLogic.rewards[0].x, gameLogic.rewards[0].y, -1);
    RewardEventHandler(gameLogic.rewards[1].x, gameLogic.rewards[1].y, -1);

    gameState.resetScores();
    gameState.setGameRunning(true);
   
}


void gameFinishedEventHandler() // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
	log("gameFinishedEventHandler fired.");
    
    sendGameFinishedEvent(0);
    sendGameFinishedEvent(1);

    gameLogic.reset();
    gameState.setGameRunning(false);
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
        gameLogic.reset();
        gameState.setGameRunning(false);
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
        gameLogic.reset();
        gameState.setGameRunning(false);
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
	else
	{
		log("Error (if any): " + err);
		server.wsSend(clientID, "Received unexpected message: " + message);
	}
}


/* Called 100 times every 1 second */
void periodicHandler()
{
    static int count = 0;
    count++;

    // Call functions once every (20/100) seconds
    if (count==20)
    {
        // Only when the game is currently running.
        if(gameState.getGameRunning())
        {
            if(gameLogic.determine_winner()!=-1)
            {
                gameFinishedEventHandler();
            }
            else
            {    
                // Locally check the rewards collision, delete corresponding reward and then update local player's score
                // result[0] is the player who scores,  result[1] is the index of the reward for the client to remove.
                rewardInfo ri = gameLogic.process_rewards();
            
                //gameLogic.print_it();
                // If two rewards are eaten at the same time, p1 is first processed, p2 will be processed next loop
                if(ri.player!=-1)
                {
                    RewardEventHandler(ri.new_reward.x, ri.new_reward.y, ri.index);
                    playerScoreEventHandler(ri.player);
                }
                           
                // Ask the clients to move and then the server to move
                loopEventHandler();
                gameLogic.move();
                
             }
        }        

        count = 0;
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
    server.setPeriodicHandler(periodicHandler);
    
	server.startServer(port);
 
    std::cout<<"hello\n";      
	return 0;
}
