const COLS = 30;
const ROWS = 30;
const cell_dim = 15;              // Size of a dot
const width = COLS * cell_dim;
const height = ROWS * cell_dim;

const init_snake_length = 10;      // Default length of snake

const INTERVAL = 60;            // Loop every 60 milliseconds

                
const LEFT = -1;           // If a snake is going left, its x variable will be add LEFT, that is -1, for each loop. 
const RIGHT = 1;           // And when the snake goes left or right, its vertical direction will add NONE, that is 0.
const UP = -1;
const DOWN = 1;
const NONE = 0;

const KEY_UP = 38;
const KEY_DOWN = 40;
const KEY_RIGHT = 39;
const KEY_LEFT = 37;

const KEY_W = 87;
const KEY_S = 83;
const KEY_D = 68;
const KEY_A = 65;

var canvas;
var ctx;
var game_interval_ID;       // When the game should be ended, this ID is passed to the function clearInterval(ID) to end the loop.

var score;

var p1_Hori;     // Horizontal velocity of player 1 snake, when receiving an input, for example, LEFT_ARROW from keyboard, p1_Hori will be set to LEFT;
var p1_Vert;     // Vertical velocity of player 1 snake, when receiving 
var p2_Hori;
var p2_Vert;
  
var p1snake;
var p2snake;

function init_canvas() 
{
    // Create and initiate the canvas element
    canvas = document.createElement("canvas");
    canvas.width = width;
    canvas.height = height;
    ctx = canvas.getContext("2d");

    // Add the canvas element to the body of the document
    document.body.appendChild(canvas);
}

// At the beginning of the game, the top left snake goes right, the bottom right snake goes left.
function init_direction()
{
    p1_Hori = RIGHT;
    p1_Vert = NONE;
    
    p2_Hori = LEFT;
    p2_Vert = NONE;
}

// It detects collision and process the situation when a snake gets a reward 
function update()
{
    if(detect_out_of_bound()===-1 || detect_snake_collision()===-1)
    {
        clearInterval(game_interval_ID);
    }
}

function loop()
{
    update();
    blit(); 
    move();
}


// If a snake goes left, it cannot go right.
function init_input()
{
    document.addEventListener('keydown', function(e)
    {   
        if(e.keyCode === KEY_DOWN && p1_Vert != UP) {
            p1_Hori = NONE;
            p1_Vert = DOWN;
        } else if(e.keyCode === KEY_UP && p1_Vert != DOWN) {
            p1_Hori = NONE;
            p1_Vert = UP;
        } else if(e.keyCode === KEY_LEFT && p1_Hori != RIGHT){
            p1_Hori = LEFT;
            p1_Vert = NONE;
        } else if(e.keyCode === KEY_RIGHT && p1_Hori != LEFT){
            p1_Hori = RIGHT;
            p1_Vert = NONE;
        } else if(e.keyCode === KEY_S && p2_Vert != UP) {
            p2_Hori = NONE;
            p2_Vert = DOWN;
        } else if(e.keyCode === KEY_W && p2_Vert != DOWN) {
            p2_Hori = NONE;
            p2_Vert = UP;
        } else if(e.keyCode === KEY_A && p2_Hori != RIGHT) {
            p2_Hori = LEFT;
            p2_Vert = NONE;
        } else if(e.keyCode === KEY_D && p2_Hori != LEFT) {
            p2_Hori = RIGHT;
            p2_Vert = NONE;
        }

    },false);
}

function main()
{
    init_canvas();
    init_snakes();
    init_direction();
    init_input();     
    
    // Set the loop function to be called every x milliseconds, x to be INTERVAL.
    game_interval_ID = setInterval(loop,INTERVAL);
    loop();
}

// Creating snakes array and posistions
// Snake have the structures like [{x:10,y:0},{x:11,y:0},{x:12,y:0}], each dictionary represents a unit of the snake, x,y are positions.
// The board is a 30x30 space, start with the first row 0, and last row 29, first column 0 and last column 29
function init_snakes()
{
	p1snake = [];
	p2snake = [];
	
    // J is used to have the right "head" as starting position, see move() for detail
	var j = init_snake_length-1; 
	for(var i = 0; i < init_snake_length; i++)
	{	
		// Initializing player 1 at upper left 
		p1snake.push({x:j--, y:0});
       		
		// Initializing player 2 at lower right
		p2snake.push({x:COLS-init_snake_length+i, y:ROWS-1});
	}
}

	
// Blitting (drawing) the pixels onto the screen
function blit()
{
    ctx.fillStyle = "white";
	ctx.fillRect(0,0,width,height);
	ctx.strokeStyle = "black";
	ctx.strokeRect(0,0, width, height);

	for(var i = 0; i<p1snake.length; i++){
		var pos = p1snake[i];
		fill(pos.x, pos.y, "black");
	}
	
	for(var i =0; i<p2snake.length;i++){
		var pos = p2snake[i];
		fill(pos.x, pos.y, "blue");
	}
}

function move()
{
    // Delete the end for each snake.
    p1snake.pop();
    p2snake.pop();
    
    // Add new head for each snake. The new head should be one unit forward towards the direction
    // The direction is either -1,0,1, updated automatically when receiving input. The input listener is set up
    // in the function init_input().
    p1snake.unshift( {x:p1snake[0].x + p1_Hori,
                      y:p1snake[0].y + p1_Vert} );
    p2snake.unshift( {x:p2snake[0].x + p2_Hori,
                      y:p2snake[0].y + p2_Vert} );       
}

// Filling a dot that can be the snake body or the food
function fill(x,y, color) 
{
	ctx.fillStyle = color;
	ctx.fillRect(x*cell_dim, y*cell_dim, cell_dim, cell_dim);
}

// Return -1 when one of the snake goes out of bound, return 0 when no snake out of bound. Simultaneous out of bound not yet considered.
function detect_out_of_bound()
{   
    if(p1snake[0].x<0 || p1snake[0].x>30 || p1snake[0].y<0 || p1snake[0].y>30)
    {
        return -1;
    } else if (p2snake[0].x<0 || p2snake[0].x>30 || p2snake[0].y<0 || p2snake[0].y>30)
    {
        return -1;
    }
    return 0;
}

// Return -1 when one of the snakes run into the other
// Loop each snake's head looking for if there is any snake head's position is 
// equal to one of the node of its body or the node of the other snake's body.
function detect_snake_collision()
{
    // Get the position of the head of p1 snake.
    var p1_x = p1snake[0].x;
    var p1_y = p1snake[0].y;
    
    for(var i=0; i<p2snake.length; i++)
    {
        // Detect collision with itself
        if(i!=0 && p1_x === p1snake[i].x && p1_y === p1snake[i].y)
        {
            return -1;
        }

        // Detect collision with the other snake
        if(p1_x === p2snake[i].x && p1_y === p2snake[i].y) 
        {
            return -1;
        }
    }
    
    var p2_x = p2snake[0].x;
    var p2_y = p2snake[0].y;

    for(var i=0; i<p1snake.length; i++)
    {
        if(i!=0 && p2_x === p2snake[i].x && p2_y === p2snake[i].y)
        {
            return -1;
        }
        if(p2_x === p1snake[i].x && p2_y === p1snake[i].y) 
        {
            return -1;
        }
    }

    return 0;
}
