/*
	ICS 167 | Group 2
	Project Server | gamestate.cpp
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

//NEWLY ADDED
int GameState::getPlayerScore(int player)
{
	return (player == PLAYER_1 ? this->player1Score : this->player2Score);
}

std::string GameState::getPlayerDirection(int player)
{
	return (player == PLAYER_1 ? this->player1Direction : this->player2Direction);	
}
bool GameState::getPlayer1Online()
{
	return this->player1Online;
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
		this->player1Online = true;
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

///NEWLY ADDED
void GameState::setPlayerDirection(int player, std::string direction)
{
	if (player == PLAYER_1)
	{
		player1Direction = direction;
	}
	else if(player == PLAYER_2) {
		player2Direction = direction;
	}	
}


void GameState::resetScores()
{
	this->player1Score = 0;
	this->player2Score = 0;
}

void GameState::resetIDs()
{
	this->player1ID = "Player 1";
	this->player2ID = "Player 2";
}

void GameState::reset()
{
	this->resetIDs();
	this->resetScores();
}
