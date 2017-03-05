/*
	ICS 167 | Group 2
	Project Server | gamelogic.h
*/


#ifndef GAMELOGICPROJECTICS167
#define GAMELOGICPROJECTICS167

#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <cstdlib>

/* Dimension of board */
const int COLS = 30;
const int ROWS = 30;

/* Direction Values: the snake's head x coordinates will add LEFT or RIGHT for each loop, similarly for vertical*/
const int LEFT  = -1;           
const int RIGHT = 1;           
const int UP    = -1;
const int DOWN  = 1;
const int NONE  = 0;

/* Literal flags */
const int NOT_COLLIDE = -1;


struct dot{
	int x;
	int y;

    dot(int _x=-1, int _y=-1): x(_x), y(_y)
    {}
};

// Information for which player eats which reward and the coordinates of the new reward
struct rewardInfo {
    int player;  // 1 for player 1,   2 for player 2
       
    dot del_reward;
    dot new_reward;

    rewardInfo(int _player, dot _del_reward, dot _new_reward): player(_player)
    {
        del_reward.x = _del_reward.x;
        del_reward.y = _del_reward.y;

        new_reward.x = _new_reward.x;
        new_reward.y = _new_reward.y;
    }
};

class GameLogic{

public:
    /* functions */
	GameLogic();
    
    std::string bodyToString(int player);
    
    bool setDirection(int player, std::string direction);  
    int incrementScore(int player);     // Increment and then return the current score.
    
    int determine_winner();             // Check all the collision and set according winner if there is any
    dot randomize_reward();             // Append a random reward at the end of rewards and return the coordinates of the new reward
    void move();                        // Add a new head and delete the end.
    rewardInfo process_rewards();       // Delete the reward a snake has eaten. Only process 1 snake at a time if there is any has eaten a reward.
    void reset();                       // Reset obstacles, rewards, snakes and variables
    std::string getWinner();            // Return the winner as a string "tie", "p1", "p2"

    /* objects */
    std::vector<dot> rewards;

    int frame = 0;
    int round = -1;
    int Round;

private: 
    /* obstacles */
	void init_obstacles();
	void init_rewards();
    void init_snakes();
    void create_obstacle(int x, int y, int size, std::string direction);
	
    /* collision detection */
	int detect_out_of_bound(std::vector<dot> obj);
	int detect_collision(std::vector<dot> obj1, std::vector<dot> obj2, int n);
	int detect_snake_collision();
    //std::vector<int> detect_rewards();   // Check if any snake eats an item and returns corresponding reward information for each snake
    
    /* helper */
	void add_tail(std::vector<dot>& obj);
    int random_number(int floor, int ceiling);    // [x,y]


    /* objects */
	std::vector<dot> obstacles; 
	std::vector<dot> p1snake; 
	std::vector<dot> p2snake; 
	


   	int p1_Hori = RIGHT;
	int p1_Vert = NONE;
	int p2_Hori = LEFT;
	int p2_Vert = NONE;
	int p1_score = 0;
	int p2_score = 0;
	bool p1_win = false;
    bool p2_win = false;
    bool tie_game = false;
};

#endif
