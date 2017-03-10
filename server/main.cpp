/*
	ICS 167 | Group 2
	Project Server | main.cpp
*/

#include "websocket.h" // Library authored by the TA.
#include "json11.hpp" // Open-source C++ JSON parser library by MIT at https://github.com/dropbox/json11
#include "gamestate.h" // Class created to keep track of game state.
#include "gamelogic.h"
#include "latency.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

typedef json11::Json JSON;

/* Begin Variables, Structs & Classes */

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

bool contains(std::vector<int> clientIDs, int clientID)
{
	return std::find(clientIDs.begin(), clientIDs.end(), clientID) != clientIDs.end();
}

/* Begin Event Senders */

void sendLoopEvent(int clientID, int frame)
{
    JSON msg_obj = JSON::object{ {"event", "loopEvent"},
                                 {"body1", gameLogic.bodyToString(1)},
                                 {"body2", gameLogic.bodyToString(2)},
                                 {"frame", frame},
                                 {"round", gameLogic.round},};
    lat_msg.push(info(clientID, msg_obj.dump()));
}

void sendUpdatePlayerNumberEvent(int clientID, int player) // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
    JSON msg_obj = JSON::object{ {"event", "updatePlayerNumberEvent"},
                                 {"player", std::to_string(player) }, };
    lat_msg.push(info(clientID, msg_obj.dump()));
    
    log("sendUpdatePlayerNumberEvent sent for client ID " + std::to_string(clientID) + " (player# = " + std::to_string(player) + ").");
}

void sendPlayerDisconnectEvent(int clientID, int player) // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
    JSON msg_obj = JSON::object{ {"event", "playerDisconnectEvent"},
                                 {"player", std::to_string(player)}, };
    lat_msg.push(info(clientID, msg_obj.dump()));
    
    log("playerDisconnectEvent sent for client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " disconnected.)");
}

void sendPlayerConnectedEvent(int clientID, int player, std::string id)
{
	JSON msg_obj = JSON::object{ {"event", "playerConnectedEvent"},
								 {"player", std::to_string(player)},
								 {"id", id}, };
    lat_msg.push(info(clientID, msg_obj.dump()));
            
    log("playerConnectedEvent sent for client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " connected.)");
}

void sendGameStartedEvent(int clientID)
{
    JSON msg_obj = JSON::object{ {"event", "gameStartedEvent"} };
    lat_msg.push(info(clientID, msg_obj.dump()));    
   
    log("gameStartedEvent sent for client ID " + std::to_string(clientID) + ")");
}

void sendPlayerDirectionEvent(int clientID, int player, std::string direction, int frame)
{
    JSON msg_obj = JSON::object{ {"event", "playerDirectionEvent"},
                                 {"direction", direction},
                                 {"player", std::to_string(player)},
                                 {"frame", frame},
                                 {"round", gameLogic.round},};
    lat_msg.push(info(clientID, msg_obj.dump()));
    
    //log("playerDirectionEvent sent to client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " moved " + direction + ")");
}

void sendNewRewardEvent(int clientID, int new_x, int new_y, int del_x, int del_y)
{
    JSON msg_obj = JSON::object{ {"event", "newRewardEvent"},
                                 {"new_x", std::to_string(new_x)},
                                 {"new_y", std::to_string(new_y)},
                                 {"del_x", std::to_string(del_x)},
                                 {"del_y", std::to_string(del_y)},
                                 {"round", gameLogic.round},};
    lat_msg.push(info(clientID, msg_obj.dump()));

    log("newRewardEvent sent to client ID " + std::to_string(clientID) + " (new_x: " + std::to_string(new_x) + ", new_y: " + std::to_string(new_y) + ", del_x: " + std::to_string(del_x) + ", del_y: " + std::to_string(del_y) + ").");
}

void sendGameFinishedEvent(int clientID)
{
    JSON msg_obj = JSON::object{ {"event", "gameFinishedEvent"},
                                 {"winner", gameLogic.getWinner()},};
    lat_msg.push(info(clientID, msg_obj.dump()));

    log("gameFinishedEvent sent to client: "+ std::to_string(clientID));
}

void sendPlayerScoreRelayEvent(int clientID, int player)
{
    JSON msg_obj = JSON::object{ {"event", "playerScoreRelayEvent"},
                                 {"player", std::to_string(player)},
                                 {"round", gameLogic.round},};
    lat_msg.push(info(clientID, msg_obj.dump()));

    log("playerScoreRelayEvent sent to client ID " + std::to_string(clientID) + " (Player: " + std::to_string(player) + ").");
}

void sendLatencyEstimationEvent(int clientID, int latencyID, long long X, long long Y)
{
    JSON msg_obj = JSON::object{ {"event", "latencyEstimationEvent"},
                                 {"id", std::to_string(latencyID)},
                                 {"X",  std::to_string(X)},
                                 {"Y",  std::to_string(Y)},};
    
    lat_msg.push(info(clientID, msg_obj.dump()));
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
			
			new_event("New player ID set for " + previousID + ": " + id);
			sendUpdatePlayerNumberEvent(clientID, 1);
		}
		else
		{
			log("Player 2 has connected.");
			std::string previousID = gameState.getPlayerID(2);
			gameState.setPlayerID(2, id);
			
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
   	log("PlayerDirectionEventHandler fired: "+std::to_string(player)+"-"+direction);
	if (gameLogic.setDirection(player, direction)) //If direction change is valid
	{
		sendPlayerDirectionEvent(0, player, direction, gameLogic.frame);
		sendPlayerDirectionEvent(1, player, direction, gameLogic.frame);
	}
	else 
	{
		log("Invalid Move");
	}
}

void loopEventHandler(int frame)
{
    sendLoopEvent(0,frame);
    sendLoopEvent(1,frame);
}

void playerScoreEventHandler(int player)
{ 
    // Increment and then broacast the player who scores
	log("playerScoreEventHandler fired.");
	
    gameLogic.incrementScore(player);

	sendPlayerScoreRelayEvent(0, player);
	sendPlayerScoreRelayEvent(1, player);
	
   	new_event(gameState.getPlayerID(player) + " scored. New score: " + std::to_string(gameState.getPlayerScore(player)));
}

void RewardEventHandler(int new_x, int new_y, int del_x, int del_y)
{
	log("RewardEventHandler fired.");
	sendNewRewardEvent(0, new_x, new_y, del_x, del_y);
	sendNewRewardEvent(1, new_x, new_y, del_x, del_y);
}

void gameStartEventHandler()
{
	log("gameStartEventHandler fired.");
	new_event("Player scores have been set back to 0 (client started game.)");

	sendGameStartedEvent(0);
	sendGameStartedEvent(1);

    // Send the information of the randomized rewards to clients, -1 indicates there is no reward to be removed.
    RewardEventHandler(gameLogic.rewards[0].x, gameLogic.rewards[0].y, -1,-1);
    RewardEventHandler(gameLogic.rewards[1].x, gameLogic.rewards[1].y, -1,-1);
    gameLogic.round++;
    
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

void NTPEventHandler(int clientID, int latencyID)
{
    long long X = currentTime();

    long long Y = currentTime();

    sendLatencyEstimationEvent(clientID, latencyID, X, Y);
}

/* Begin Network Handlers */

void openHandler(int clientID)
{
	log("Connection opened. Client ID: " + std::to_string(clientID));
	std::vector<int> clientIDs = server.getClientIDs();

	if (clientIDs.size() > 2)
	{
		log("Rejecting connection..");
        lat_msg.push(info(clientID, "There are already two clients connected. Rejecting your connection.."));
		//server.wsSend(clientID, "There are already two clients connected. Rejecting your connection..");
		server.wsClose(clientID);
		log("Connection forcefully closed. (Reason: two clients already exist.) Client ID: " + std::to_string(clientID));
	} 
	else
	{
		log("Connection established.");
        lat_msg.push(info(clientID, "Connection established."));
		//server.wsSend(clientID, "Connection established.");
	}
}

void closeHandler(int clientID) 
{
	log("Connection closed. Client ID: " + std::to_string(clientID));
	std::vector<int> clientIDs = server.getClientIDs();
	lat_msg.push(info(clientID, "Connection closed."));
    //server.wsSend(clientID, "Connection closed.");
	if (clientID == 0) // the main client
	{
		if (clientIDs.size() >= 2)
		{
			sendPlayerDisconnectEvent(1, PLAYER_1); // Tell Client ID 1 (Player 2) that Player 1 has disconnected.
			server.wsClose(1);
		}

        gameLogic.reset();
        gameLogic.round = -1;

		gameState.setPlayer1Online(false);
		gameState.resetScores();
        
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
    
        gameLogic.reset();
        gameLogic.round = -1;
    
		gameState.resetScores();
        gameState.setGameRunning(false);
		log(gameState.getPlayerID(PLAYER_2) + " has disconnected (Player #2)." + " Game state has been completely reset.");
		gameState.resetID(PLAYER_2);
	}
}

// When the server receives message from clients, push it to lat_event to be processed later after some delay.
void messageHandler(int clientID, std::string message) 
{
    lat_event.push(info(clientID, message));
}

void processEvent(info &i)
{
    //log("Message received from Client ID " + std::to_string(clientID) + ": " + message);

	std::string err; // This string is updated with an error message if the json parser fails.
	auto json = JSON::parse(i.msg, err);
	std::string firedEvent = json["event"].string_value();
	
    std::vector<int> clientIDs = server.getClientIDs();
    int clientID = i.clientID;

	// Begin Event Handling
	if (firedEvent == "setPlayerIDEvent") // JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
	{
		setPlayerIDEventHandler(clientID, clientIDs.size(), json["id"].string_value());
	}
	else if (firedEvent == "setPlayerDirectionEvent")
	{
        if(gameState.getGameRunning())
		    setPlayerDirectionEventHandler(clientID, json["player"].int_value(), json["direction"].string_value());
	}
	else if (firedEvent == "gameStartEvent") // JSON example -> {"event": "gameFinishedEvent"}
	{
		gameStartEventHandler();
	}
	else if (firedEvent == "latencyEstimationEvent")
	{
        NTPEventHandler(clientID, json["id"].int_value());
    }
	else
	{
		log("Error (if any): " + err);
		lat_msg.push(info(clientID, "Received unexpected message: " + i.msg));
    }

}

// Check if any event should now be processed.
void checkEvent()
{
    std::vector<int> clientIDs = server.getClientIDs();

    while (lat_event.size()!=0)
    {
        info i = lat_event.top();
	    if (i.timeToProcess > currentTime())  break;
	   
	    if ( (i.clientID == 0 && contains(clientIDs, 0)) || (i.clientID == 1 && contains(clientIDs, 1)) )
	    {
	        processEvent(i);
	    }
	    lat_event.pop();
    }
}

// Check if there is any message to be sent to clients.
void checkMessage()
{
    std::vector<int> clientIDs = server.getClientIDs();

    while (lat_msg.size()!=0)
    {
        info i = lat_msg.top();
	    if (i.timeToProcess > currentTime())  break;
	   
	    if ( (i.clientID == 0 && contains(clientIDs, 0)) || (i.clientID == 1 && contains(clientIDs, 1)) )
	    {
	        server.wsSend(i.clientID, i.msg);
	    }
	    lat_msg.pop();
    }
}

// Send the clients the snake's position once every 10/100 seconds
void sendPosition()
{
    static int update_tick = 0;

    if (update_tick++ == 10)          
    {
        if (gameState.getGameRunning())
        {
            loopEventHandler(gameLogic.frame);     
        }
       
        update_tick = 0;
    }
}

// Loop the game once every 10/100 seconds
void gameLoop()
{
    static int logic_tick = 0;  
    if (logic_tick++ == 10)
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
		        // For every loop, only one reward eaten is processed, if two snakes eat a reward at the same time,
		        // the player 2 snake will have to wait till next loop.
                // ri is the information of the reward just eaten. ri.player: who eats the reward.
		        // ri.index: the index of the reward just eaten.  ri.new_reward: the new_reward that has been appended to rewards.
                rewardInfo ri = gameLogic.process_rewards();
            
                //gameLogic.print_it();
                // If two rewards are eaten at the same time, p1 is first processed, p2 will be processed next loop
                if(ri.player!=-1)
                {
                    RewardEventHandler(ri.new_reward.x, ri.new_reward.y, ri.del_reward.x, ri.del_reward.y);
                    playerScoreEventHandler(ri.player);
                }
                                      		    
		        // Move the snake locally.
                gameLogic.move();
                gameLogic.frame++;
               
            }
        }        
        logic_tick = 0;
    }
}

/* Called 100 times every 1 second */
void periodicHandler()
{
    checkEvent();         // Check if any event ready to be processed, if so, process.
    checkMessage();       // Check if any message to client is ready to be sent, if so, sent.
    gameLoop();           // Simulates the game by one loop
    sendPosition();         // Send the positions of two snakes to two clients   
}

/* Begin Main Function */
int main()
{
	int port;

	std::cout << "[INIT] Please enter the desired port: ";
	std::cin >> port;

    srand(time(NULL));

	server.setOpenHandler(openHandler);
	server.setCloseHandler(closeHandler);
	server.setMessageHandler(messageHandler);
    server.setPeriodicHandler(periodicHandler);
    
	server.startServer(port);
 
          
	return 0;
}
