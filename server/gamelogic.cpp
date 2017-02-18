/*
	ICS 167 | Group 2
	Project Server | gamelogic.cpp
*/

#include "gamelogic.h"
#include<string>

GameLogic::GameLogic()
{
	///most likely place to send obstacles and rewards information upon game start


	p1_win = false;
	p2_win = false;
	tie_game = false;
	
	init_snakes();
	init_obstacles();
	init_rewards();
}

/// Create Snake/////
void GameLogic::init_snakes()
{
	int snakeHead = 1;
	for (int i = 0; i < 2; i++)
	{
		p1snake.push_back(dot());
		p1snake[i].x = snakeHead;
		p1snake[i].y = 0;
		snakeHead--;

		p2snake.push_back(dot());
		p2snake[i].x = COLS - 2 + i;
		p2snake[i].y = ROWS - 1;
	}

	p1_Hori = RIGHT;
	p1_Vert = NONE;

	p2_Hori = LEFT;
	p2_Vert = NONE;

	p1_score = 0;
	p2_score = 0;

}

///MOVE///

void GameLogic::move()
{
	p1snake.pop_back();
	p2snake.pop_back();

	p1snake.insert(p1snake.begin(), dot());
	p2snake.insert(p2snake.begin(), dot());

	p1snake[0].x = p1snake[1].x + p1_Hori;
	p1snake[0].y = p1snake[1].y + p1_Vert;

	p2snake[0].x = p2snake[1].x + p2_Hori;
	p2snake[0].y = p2snake[1].y + p2_Vert;

}



////Generate Obstacles///
void GameLogic::init_obstacles()
{
	create_obstacle(5, 5, 4, "Vertical");
	create_obstacle(10, 10, 7, "Vertical");
	create_obstacle(20, 20, 5, "Horizontal");
	create_obstacle(18, 10, 5, "Horizontal");
}

void GameLogic::create_obstacle(int x, int y, int size, std::string direction)
{
	if (direction == "Horizontal")
	{
		for (int end = x + size; x < end; x++)
		{
			obstacles.push_back(dot());
			obstacles[end].x = x;
			obstacles[end].y = y;
		}
	}

	else if (direction == "Vertical")
	{
		for (int end = y + size; y < end; y++)
		{
			obstacles.push_back(dot());
			obstacles[end].x = x;
			obstacles[end].y = y;
		}
	}
}


///Generate Reward///

void GameLogic::init_rewards()
{
	for (int i = 0; i < 2; i++)
	{
		randomize_reward();
	}
}

void GameLogic::randomize_reward()
{
	std::vector<dot> random_pos;

	do{
		random_pos = { dot() };
		random_pos[0].x = rand() % 29;
		random_pos[0].y = rand() % 29;
	} while (detect_collision(random_pos, p1snake, 0) != NOT_COLLIDE ||
		detect_collision(random_pos, p2snake, 0) != NOT_COLLIDE ||
		detect_collision(random_pos, obstacles, 0) != NOT_COLLIDE);
	
	rewards.push_back(random_pos[0]);
}
	
/// CHECKING WINNER ////

int GameLogic::determine_winner()
{
	
	int return_val = -1;

	if (detect_collision(p1snake, p1snake, 1) != NOT_COLLIDE ||
		detect_collision(p1snake, p2snake, 0) != NOT_COLLIDE ||
		detect_collision(p1snake, p2snake, 0) != NOT_COLLIDE ||
		detect_out_of_bound(p1snake) != NOT_COLLIDE)
	{
		p2_win = true;
		return_val = 1;
	}

	if (detect_collision(p2snake, p2snake, 1) != NOT_COLLIDE ||
		detect_collision(p2snake, p1snake, 0) != NOT_COLLIDE ||
		detect_collision(p2snake, obstacles, 0) != NOT_COLLIDE ||
		detect_out_of_bound(p2snake) != NOT_COLLIDE)
	{
		p1_win = true;
		return_val = 1;
	}

	if (p1_win && p2_win)
	{
		if (p1_score == p2_score)
		{
			tie_game = true;
			p1_win = false;
			p2_win = false;

		}
		else if (p1_score > p2_score){
			p2_win = false;
		}
		else {
			p1_win = false;
		}
	}


	return return_val;
}

///COLLISION DETECTION/////

int GameLogic::detect_collision(std::vector<dot> obj1, std::vector<dot> obj2, int n)
{
	int x1 = obj1[0].x;
	int y1 = obj1[0].y;

	for (int i = n; i < obj2.size(); i++)
	{
		if (x1 == obj2[i].x && y1 == obj2[i].y)
		{
			return i;
		}
	}

	return NOT_COLLIDE;
}

int GameLogic::detect_out_of_bound(std::vector<dot> obj)
{
	return (obj[0].x <0 || obj[0].x > COLS - 1 || obj[0].y < 0 || obj[0].y > ROWS - 1)? 1: NOT_COLLIDE;
}

int GameLogic::detect_snake_collision()
{
	return (detect_collision(p1snake, p1snake, 1) != NOT_COLLIDE || detect_collision(p2snake, p2snake, 1) != NOT_COLLIDE ||
		detect_collision(p1snake, p2snake, 0) != NOT_COLLIDE || detect_collision(p2snake, p1snake, 0) != NOT_COLLIDE) ? 1 : NOT_COLLIDE;
}


