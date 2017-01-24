/*
	ICS 167 | Group 2
	Project Server | gamestate.cpp
	Author: Thomas T Nguyen
*/

#include "gamestate.h"
#include <string>

GameState::GameState()
{

}

GameState::GameState(std::string player1ID, std::string player2ID)
{
	this->player1ID = player1ID;
	this->player2ID = player2ID;
}

std::string GameState::getPlayerID(int player)
{
	return (player == PLAYER_1 ? player1ID : player2ID);
}

int GameState::getPlayerScore(int player)
{
	return (player == PLAYER_1 ? player1Score : player2Score);
}

int GameState::incrementScore(int player)
{
	if (player == PLAYER_1)
	{
		player1Score++;
		return player1Score;
	}
	else
	{
		player2Score++;
		return player2Score;
	}
}

void GameState::setPlayerID(int player, std::string id)
{
	if (player == PLAYER_1)
	{
		player1ID = id;
	}
	else player2ID = id;
}


void GameState::setPlayerScore(int player, int score)
{
	if (player == PLAYER_1)
	{
		player1Score = score;
	}
	else player2Score = score;
}