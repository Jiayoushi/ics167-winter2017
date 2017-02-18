/*
	ICS 167 | Group 2
	Project Server | gamestate.cpp
*/


#ifndef GAMELOGICPROJECTICS167
#define GAMELOGICPROJECTICS167

#include<string>
#include<vector>

struct dot{
	int x;
	int y;
};


class GameLogic{

public:
	GameLogic(); //constructor

	void init_obstacles();
	void create_obstacle(int x, int y, int size, std::string direction);

	void init_rewards();
	void randomize_reward();

	void determine_winner();

	int detect_collision(std::vector<dot> obj1, std::vector<dot> obj2, int n);


private:

	std::vector<dot> obstacles; 
	std::vector<dot> rewards;

	std::vector<dot> p1snake; /// Will need to be change later to class
	std::vector<dot> p2snake;


	//keyboard matching
	const int LEFT = -1;           // If a snake is going left, its x variable will be add LEFT, that is -1, for each loop. 
	const int RIGHT = 1;           // And when the snake goes left or right, its vertical direction will add NONE, that is 0.
	const int UP = -1;
	const int DOWN = 1;
	const int NONE = 0;

	/* Literal flags */
	const int NOT_COLLIDE = -1;



};






#endif