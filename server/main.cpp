/*
	ICS 167 | Group 2
	Project Server | main.cpp
*/

#include "websocket.h" // Library authored by the TA.
#include "json11.hpp" // Open-source C++ JSON parser library by MIT at https://github.com/dropbox/json11
#include "gamestate.h" // Class created to keep track of game state.
#include "gamelogic.h"

#include <algorithm>
#include <queue>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

typedef json11::Json JSON;

/* Begin Variables, Structs & Classes */

const int DELAY = 200;		// Milliseconds of delay

webSocket server;
GameState gameState;
GameLogic gameLogic;

long long currentTime()
{
	return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

struct info 
{
    long long timeToSend;
    int clientID;
    std::string msg;

    info(int _clientID, std::string _msg): 
                clientID(_clientID), msg(_msg)
    {
	    timeToSend = currentTime() + rand()%DELAY;
    }
};

struct NTP 
{
	long long timeToSend;           // The time that the server should send the message, it is not the actual sending time
	int clientID;
	int latencyID;
	long long X;

	NTP(int _clientID, int _latencyID, long long _X) : 
            clientID(_clientID), latencyID(_latencyID), X(_X)
	{
		timeToSend = currentTime() + rand()%DELAY;
	}
};

template<typename T>
struct greaterThanByTime {
	bool operator()(const T &lhs, const T &rhs){
		return lhs.timeToSend > rhs.timeToSend;
	}
};


std::priority_queue<info, vector<info>, greaterThanByTime<info>> lat;
std::priority_queue<NTP, vector<NTP>, greaterThanByTime<NTP>> lat_est;

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

void sendLoopEvent(int clientID)
{
    lat.push(info(clientID, "{\"event\": \"loopEvent\"}"));
    //server.wsSend(clientID, "{\"event\": \"loopEvent\"}");
}

void sendUpdatePlayerNumberEvent(int clientID, int player) // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
    lat.push(info(clientID, "{\"event\": \"updatePlayerNumberEvent\", \"player\": " + std::to_string(player) + "}"));
	//server.wsSend(clientID, "{\"event\": \"updatePlayerNumberEvent\", \"player\": " + std::to_string(player) + "}");
	log("sendUpdatePlayerNumberEvent sent for client ID " + std::to_string(clientID) + " (player# = " + std::to_string(player) + ").");
}

void sendPlayerDisconnectEvent(int clientID, int player) // in the future, the game logic will be handled by the server itself (prob milestone 2)
{
    lat.push(info(clientID, "{\"event\": \"playerDisconnectEvent\", \"player\": " + std::to_string(player) + "}"));
	//server.wsSend(clientID, "{\"event\": \"playerDisconnectEvent\", \"player\": " + std::to_string(player) + "}");
	log("playerDisconnectEvent sent for client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " disconnected.)");
}

void sendPlayerConnectedEvent(int clientID, int player, std::string id)
{
    lat.push(info(clientID, "{\"event\": \"playerConnectedEvent\", \"id\":\"" + id + "\", \"player\": " + std::to_string(player) + "}"));
	//server.wsSend(clientID, "{\"event\": \"playerConnectedEvent\", \"id\":\"" + id + "\", \"player\": " + std::to_string(player) + "}");
	log("playerConnectedEvent sent for client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " connected.)");
}

void sendGameStartedEvent(int clientID)
{
    //lat.push(info(clientID, "The game has been started!"));
    lat.push(info(clientID, "{\"event\": \"gameStartedEvent\"}"));
	//server.wsSend(clientID, "The game has been started!");
	//server.wsSend(clientID, "{\"event\": \"gameStartedEvent\"}");
	log("gameStartedEvent sent for client ID " + std::to_string(clientID) + ")");
}

void sendPlayerDirectionEvent(int clientID, int player, std::string direction)
{
    lat.push(info(clientID, "{\"event\": \"playerDirectionEvent\", \"direction\":\"" + direction + "\", \"player\": " + std::to_string(player) + "}"));
	//erver.wsSend(clientID, "{\"event\": \"playerDirectionEvent\", \"direction\":\"" + direction + "\", \"player\": " + std::to_string(player) + "}");
	log("playerDirectionEvent sent to client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " moved " + direction + ")");
}

void sendNewRewardEvent(int clientID, int new_x, int new_y, int del_x, int del_y)
{
    lat.push(info(clientID, "{\"event\": \"newRewardEvent\", \"new_x\": " + std::to_string(new_x) + ", \"new_y\": " + std::to_string(new_y) + ", \"del_x\": " + std::to_string(del_x) + ", \"del_y\": " + std::to_string(del_y) + "}"));
	//server.wsSend(clientID, "{\"event\": \"newRewardEvent\", \"x\": " + std::to_string(X) + ", \"y\": " + std::to_string(Y) + ", \"index\": " + std::to_string(index) + "}");
	log("newRewardEvent sent to client ID " + std::to_string(clientID) + " (new_x: " + std::to_string(new_x) + ", new_y: " + std::to_string(new_y) + ", del_x: " + std::to_string(del_x) + ", del_y: " + std::to_string(del_y) + ").");
}

void sendGameFinishedEvent(int clientID)
{
    std::string winner = gameLogic.getWinner();
    lat.push(info(clientID, "{\"event\": \"gameFinishedEvent\", \"winner\":\"" + winner + "\"}"));
    //server.wsSend(clientID, "{\"event\": \"gameFinishedEvent\", \"winner\":\"" + winner + "\"}");   
    log("gameFinishedEvent sent to client: "+ std::to_string(clientID));
}

void sendPlayerScoreRelayEvent(int clientID, int player)
{
    lat.push(info(clientID,  "{\"event\": \"playerScoreRelayEvent\", \"player\": " + std::to_string(player)  + "}"));
	//server.wsSend(clientID, "{\"event\": \"playerScoreRelayEvent\", \"player\": " + std::to_string(player)  + "}");
	log("playerScoreRelayEvent sent to client ID " + std::to_string(clientID) + " (Player: " + std::to_string(player) + ").");
}

void sendLatencyEstimationEvent(int clientID, int latencyID, long long X, long long Y)
{
	// For this function, DO NOT lat.push or lat_est.push!! Again, DO NOT lat.push or lat_est.push!!! DO NOT!! DONT!! BAD!! DONT DO IT!!
	server.wsSend(clientID, "{\"event\": \"latencyEstimationEvent\", \"id\": " + std::to_string(latencyID) + ", \"X\": " + std::to_string(X) + ", \"Y\": " + std::to_string(Y) + "}");
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

void playerScoreEventHandler(int player)
{ // Increment and then broacast the player who scores
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

    // -1 indicates there is no reward to be removed.
    RewardEventHandler(gameLogic.rewards[0].x, gameLogic.rewards[0].y, -1,-1);
    RewardEventHandler(gameLogic.rewards[1].x, gameLogic.rewards[1].y, -1,-1);

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
        lat.push(info(clientID, "There are already two clients connected. Rejecting your connection.."));
		//server.wsSend(clientID, "There are already two clients connected. Rejecting your connection..");
		server.wsClose(clientID);
		log("Connection forcefully closed. (Reason: two clients already exist.) Client ID: " + std::to_string(clientID));
	} 
	else
	{
		log("Connection established.");
        lat.push(info(clientID, "Connection established."));
		//server.wsSend(clientID, "Connection established.");
	}
}

void closeHandler(int clientID) 
{
	log("Connection closed. Client ID: " + std::to_string(clientID));
	std::vector<int> clientIDs = server.getClientIDs();
	lat.push(info(clientID, "Connection closed."));
    //server.wsSend(clientID, "Connection closed.");
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
	//log("Message received from Client ID " + std::to_string(clientID) + ": " + message);

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
	else if (firedEvent == "latencyEstimationEvent")
	{
		lat_est.push(NTP(clientID, json["id"].int_value(), currentTime()));
		//log("(Latency Estimation) Client ID: " + std::to_string(clientID) + " | Latency ID: " + std::to_string(json["id"].int_value()));
	}
	else
	{
		log("Error (if any): " + err);
		lat.push(info(clientID, "Received unexpected message: " + message));
        //server.wsSend(clientID, "Received unexpected message: " + message);
	}
}

void processMessage()
{
    std::vector<int> clientIDs = server.getClientIDs();

    while (lat.size()!=0)
    {
        info i = lat.top();
	    if (i.timeToSend > currentTime())  break;
	   
	    if ( (i.clientID == 0 && contains(clientIDs, 0)) || (i.clientID == 1 && contains(clientIDs, 1)) )
	    {
	        server.wsSend(i.clientID, i.msg);
	    }
	    lat.pop();
    }
}

void processLatencyEstimation()
{
	std::vector<int> clientIDs = server.getClientIDs();

	while (lat_est.size()!=0)
	{
		NTP i = lat_est.top();
		if (i.timeToSend > currentTime())  break;
		
		if ((i.clientID == 0 && contains(clientIDs, 0)) || (i.clientID == 1 && contains(clientIDs, 1)))
		{
			sendLatencyEstimationEvent(i.clientID, i.latencyID, i.X, currentTime());
		}
		lat_est.pop();
	}
}

void processLogic()
{
    static int count = 0;
    count++;

    // Call functions inside this condition once every (20/100) seconds
    // Game loop is here.
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
                           
                // Ask the clients to move
                loopEventHandler();
		    
		        // Move the snake locally.
                gameLogic.move();
                
             }
        }        

        count = 0;
    }

}

/* Called 100 times every 1 second */
void periodicHandler()
{
    processMessage();
    processLogic();
    processLatencyEstimation();
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
