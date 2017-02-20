/*
	ICS 167 | Group 2
	Project Server | gamestate.h
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
		std::string getPlayerDirection(int player); //get the current direction of player
		bool getPlayer1Online();
		void setPlayer1Online(bool value);
        bool getGameRunning();        

		int incrementScore(int player); // increments score by 1, then returns new score

		void setPlayerID(int player, std::string id); // sets player ID for a player.
		void setPlayerScore(int player, int score);  // set score for a player to score.
		void setPlayerDirection(int player, std::string direction);
        void setGameRunning(bool state);
    

		void reset(); // resets scores to 0 and ids to empty string
		void resetScores();  // resets only scores to 0
		void resetIDs(); // resets only IDs to empty string
		void resetID(int player); // resets ID for only a single player
        void resetGameRunning();
        

	private:
		std::string player1ID = "Player 1";
		std::string player2ID = "Player 2";
		int player1Score = 0;
		int player2Score = 0;
		bool player1Online = false; //True when first client connects
	    bool gameRunning = false;  // When true, the server will keep sending message to tell clients to update their images
};

#endif

