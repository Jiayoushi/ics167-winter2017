/*
	ICS 167 | Group 2
	Project Server | gamelogic.h
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
	
	int determine_winner(); //TIE_GAME NOT IMPLEMENTED
			        //Need to know what field the Snake class has.
	
	int detect_out_of_bound(std::vector<dot> obj);
	int detect_collision(std::vector<dot> obj1, std::vector<dot> obj2, int n);
	int detect_snake_collision();

	//TODO: direction/input, updating snake vector


private:

	std::vector<dot> obstacles; 
	std::vector<dot> rewards;
	
	
	//Snake p1snakeClass;    //initialize the snake class
	//Snake p2snakeClass;
	//std::vector<dot> p1snake = p1snakeClass.snake;   //make sure to assign the snake as
	//std::vector<dot> p2snake = p2snakeClass.snake;   //p1snake and p2snake because certain
						           //functions reference these var

	std::vector<dot> p1snake; /// Will need to be change later to class
	std::vector<dot> p2snake; /// 

	//winning Variables//
	bool p1_win;
	bool p2_win;
	bool tie_game;


	//Dimension of "board"
	const int COLS = 30;
	const int ROWS = 30;


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
