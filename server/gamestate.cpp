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
	return (player == PLAYER_1 ? this->player1ID : this->player2ID);
}

int GameState::getPlayerScore(int player)
{
	return (player == PLAYER_1 ? this->player1Score : this->player2Score);
}

int GameState::incrementScore(int player)
{
	return (player == PLAYER_1 ? (++(this->player1Score)) : (++(this->player2Score))); // this could cause problems but i doubt it. one liners r the best
}

void GameState::setPlayerID(int player, std::string id)
{
	if (player == PLAYER_1)
	{
		this->player1ID = id;
	}
	else this->player2ID = id;
}


void GameState::setPlayerScore(int player, int score)
{
	if (player == PLAYER_1)
	{
		this->player1Score = score;
	}
	else this->player2Score = score;
}

void GameState::reset()
{
	this->player1ID = "";
	this->player2ID = "";
	this->player1Score = 0;
	this->player2Score = 0;
}