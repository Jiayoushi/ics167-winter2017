/*
	ICS 167 | Group 2
	Project Server | gamestate.cpp
*/

#include "gamestate.h"
#include <string>

GameState::GameState()
{
    gameRunning = false;
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


bool GameState::getGameRunning()
{
    return gameRunning;
}


bool GameState::getPlayer1Online()
{
	return this->player1Online;
}

int GameState::incrementScore(int player)
{
	return (player == PLAYER_1 ? (++(this->player1Score)) : (++(this->player2Score))); // this could cause problems but i doubt it. one liners r the best
}

void GameState::setGameRunning(bool state)
{
    gameRunning = state;
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

void GameState::setPlayer1Online(bool value)
{
	this->player1Online = value;
}

void GameState::setPlayerScore(int player, int score)
{
	if (player == PLAYER_1)
	{
		this->player1Score = score;
	}
	else this->player2Score = score;
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

void GameState::resetID(int player)
{
	if (player == PLAYER_1)
	{
		this->player1ID = "Player 1";
	}
	else this->player2ID = "Player 2";
}


void GameState::resetGameRunning()
{
    gameRunning = false;
}

void GameState::reset()
{
	this->resetIDs();
	this->resetScores();
  this->resetGameRunning();
}

