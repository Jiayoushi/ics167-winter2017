/*
	ICS 167 | Group 2
	Project Server (ver. Milestone 1)
	Author: Thomas T Nguyen
*/

#include <iostream>
#include <vector>
#include "websocket.h" // Library authored by the TA.
#include <string>

/* Begin Variables */
webSocket server;
int p1_id;
int p2_id;
int p1_score = 0;
int p2_score = 0;

/* log helper */
void log(string message)
{
	std::cout << "[LOG] " << message << std::endl;
}

/* called when a client connects */
void openHandler(int clientID)
{
	log("Connection opened. Client ID: " + clientID);
	std::vector<int> clientIDs = server.getClientIDs();

	if (clientIDs.size >= 1)
	{
		server.wsSend(clientID, "There are already a machine connected. Rejecting your connection.");
		server.wsClose(clientID);
		log("Connection forcefully closed. (Reason: a connection already exists.) Client ID: " + clientID);
	} 
	else server.wsSend(clientID, "Connection initialized."); // TODO: implement ID and Score checking.
}

/* called when a client disconnects */
void closeHandler(int clientID) 
{
	log("Connection closed. Client ID: " + clientID);
	server.wsSend(clientID, "Connection closed.");
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message) 
{
	log("Client ID " + std::to_string(clientID) + "says: ");
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