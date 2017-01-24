/*
	ICS 167 | Group 2
	Project Server | gamestate.h
	Author: Thomas T Nguyen
*/

#ifndef GAMESTATEPROJECTICS167
#define GAMESTATEPROJECTICS167

#define PLAYER_1 1
#define PLAYER_2 2

#include <string>

class GameState 
{
	public:
		GameState(); // default constructor
		GameState(std::string player1ID, std::string player2ID); // constructor if you know both ids beforehand

		std::string getPlayerID(int player); // returns the id for a player
		int getPlayerScore(int player); // returns the current score for a player

		int incrementScore(int player); // increments score by 1, then returns new score

		void setPlayerID(int player, std::string id); // sets player ID for a player.
		void setPlayerScore(int player, int score);  // set score for a player to score.

		void reset(); // resets scores to 0 and ids to empty string
		void resetScores();  // resets only scores to 0
		void resetIDs(); // resets only IDs to empty string

	private:
		std::string player1ID; // note: unintialized at first.
		std::string player2ID; // note: unintialized at first.
		int player1Score = 0;
		int player2Score = 0;
};

#endif

