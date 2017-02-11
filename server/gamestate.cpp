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

int GameState::getPlayerDirection(int player)
{
	return (player == PLAYER_1 ? this->player1Direction : this->player2Direction);	
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

///NEWLY ADDED
void GameState::setPlayerDirection(int player, int input)
{
	if (player == PLAYER_1)
	{
		if (input == 37 || input == 38 || input == 39 || input == 40) //if player1 uses up,down,left,right
			this->player1direction = input;
		
		////MAPPING if player 1 used wasd
		else if(input == 65) //left
			this->player2direction = 37;
		else if(input == 87) //up
			this->player2direction = 38;
		else if(input == 68)//right
			this->player2direction = 39;
		else if(input == 83)//down
			this->player2direction = 40;
	}
	else{
		if (input == 87 || input == 83 || input == 68 || input == 65)//if player 2 uses wasd
			this->player2direction = input;
		
		////MAPPING if player 2 used up,down,left,right
		else if(input == 37) //left
			this->player2direction = 65;
		else if(input == 38) //up
			this->player2direction = 87;
		else if(input == 39)//right
			this->player2direction = 68;
		else if(input == 40)//down
			this->player2direction = 83;
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
