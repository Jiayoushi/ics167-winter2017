/*
	ICS 167 | Group 2
	Project Server | gamelogic.cpp
*/

#include "gamelogic.h"
#include <string>

GameLogic::GameLogic()
{
	///most likely place to send obstacles and rewards information upon game start

    // Don't move this, it need to be placed before generate any random items
    srand(time(NULL));
	
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
		p1snake.push_back( dot( snakeHead-- , 0 ));

		p2snake.push_back( dot( COLS -2 + i , ROWS - 1));

	}
}

///MOVE///

void GameLogic::move()
{
	p1snake.pop_back();
	p2snake.pop_back();

	p1snake.insert(p1snake.begin(), dot(p1snake[0].x + p1_Hori, p1snake[0].y + p1_Vert));
	p2snake.insert(p2snake.begin(), dot(p2snake[0].x + p2_Hori, p2snake[0].y + p2_Vert));
}

void GameLogic::add_tail(std::vector<dot>& snake)
{
    int len = snake.size();
    int x = snake[len-1].x - snake[len-2].x + snake[len-1].x;
    int y = snake[len-1].y - snake[len-2].y + snake[len-1].y;
    snake.push_back(dot(x,y));
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
        for (int i = 0; i < size; i++,x++)
        {
            obstacles.push_back(dot(x,y));
		}
	}
	else if (direction == "Vertical")
	{
        for (int i = 0; i < size; i++,y++)
        {
            obstacles.push_back(dot(x,y));
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

int GameLogic::random_number(int floor, int ceiling)
{
    return rand()%ceiling+floor;
}

dot GameLogic::randomize_reward()
{
    int i = 0;
    std::vector<dot> random_pos;
    
    do{
        if(random_pos.size()>0)
            random_pos.pop_back();
        random_pos.push_back(dot(random_number(0,29),
                                    random_number(0,29)));
        
    } while (detect_collision(random_pos, p1snake, 0) != NOT_COLLIDE ||
            detect_collision(random_pos, p2snake, 0) != NOT_COLLIDE ||
            detect_collision(random_pos, obstacles, 0) != NOT_COLLIDE);
    
    rewards.push_back(random_pos[0]);
    
    return dot(random_pos[0].x, random_pos[0].y);
}


int GameLogic::determine_winner()
{    
	int return_val = -1;

    if (detect_collision(p1snake, p1snake, 1) != NOT_COLLIDE ||
		detect_collision(p1snake, p2snake, 0) != NOT_COLLIDE ||
		detect_collision(p1snake, obstacles, 0) != NOT_COLLIDE ||
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


std::vector<int> GameLogic::detect_rewards()
{
    int index = -1;
    if ((index = detect_collision(p1snake,rewards,0)) != -1)
    {
        return std::vector<int>{1,index};    
    }
    if ((index = detect_collision(p2snake,rewards,0)) != -1)
    {
       
        return std::vector<int>{2,index};
    }
    
    return std::vector<int>();
}


void GameLogic::setDirection(int player, std::string direction)
{
    if (player == 1 ) {        
        if (direction == "DOWN" ) {
            p1_Vert = DOWN;
            p1_Hori = NONE;
        } else if (direction == "UP") {
            p1_Vert = UP;
            p1_Hori = NONE;
        } else if (direction == "LEFT") {
            p1_Hori = LEFT;
            p1_Vert = NONE;
        } else if (direction == "RIGHT" ) {
            p1_Hori = RIGHT;
            p1_Vert = NONE; 
        }
    } else {
        if (direction == "DOWN" ) {
            p2_Vert = DOWN;
            p2_Hori = NONE;
        } else if (direction == "UP") {
            p2_Vert = UP;
            p2_Hori = NONE;
        } else if (direction == "LEFT" ) {
            p2_Hori = LEFT;
            p2_Vert = NONE;
        } else if (direction == "RIGHT" ) {
            p2_Hori = RIGHT;
            p2_Vert = NONE;
        }
    }   
}

int GameLogic::incrementScore(int player)
{
    return (player==1)? ++p1_score : ++p2_score ;
}


// If two rewards eaten by separate snakes at the same time, only 1 is processed.
// The 2rd one will be processed by the next call of process_rewards() function
rewardInfo GameLogic::process_rewards()
{
       
    std::vector<int> collision = detect_rewards();

    if(collision.size()!=0)
    {
        rewards.erase(rewards.begin()+collision[1]);
        return rewardInfo(collision[0],collision[1], randomize_reward());
    }
        
    return rewardInfo(-1,-1,dot(-1,-1));
}


void GameLogic::reset()
{
    rewards.clear();
    obstacles.clear();
    p1snake.clear();
    p2snake.clear();
    
    p1_Hori = RIGHT;
    p1_Vert = NONE;
    p2_Hori = LEFT;
    p2_Vert = NONE;

    p1_score = 0;
    p2_score = 0;

    p1_win = false;
    p2_win = false;
    tie_game = false;

    init_snakes();
    init_obstacles();
    init_rewards();
   
}

std::string GameLogic::getWinner()
{
    if (p1_win) 
        return "p1";
    else if (p2_win)
        return "p2";
    else if (tie_game)
        return "tie";
}
