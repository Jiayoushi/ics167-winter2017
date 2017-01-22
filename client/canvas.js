var COLS = 30;
var ROWS = 30;
var width = COLS * 15;
var height = ROWS *15;

var canvas;
var context;

var cell_dim = 15; //size of a dot
var score;
var p1direction;
var p2direction;

var init_snake_length = 5; //default size of snake
 
var p1snake_array;
var p2snake_array;


////CHANGE: COLS*15 and ROWS*15 to a global variable

function initialize_canvas() 
{
    // create and initiate the canvas element
    canvas = document.createElement("canvas");
    canvas.width = width;
    canvas.height = height;
    ctx = canvas.getContext("2d");
    // add the canvas element to the body of the document
    document.body.appendChild(canvas);
    
    init_snakes()
    var run = setInterval(blit, 60); //primitive game loop. Possible to use other better methods.
    
}

// creating snakes array and posistions
function init_snakes()
{
	p1snake_array = [];
	p2snake_array = [];
	
	var j = init_snake_length-1; //counting from 0. So -1 will give index 4;
	for(var i = 0; i < init_snake_length; i++)
	{
		
		//drawing player 1 at upper left
		p1snake_array.push({x:j--, y:0});
        //j is used to have the right "head" as starting position, see move() for detail
		
		//drawing player2 at lower right
		p2snake_array.push({x:COLS-init_snake_length+i, y:ROWS-1});
		
	}
}

//blitting (drawing) the pixels onto the screen
function blit(){
	ctx.fillStyle = "white";
	ctx.fillRect(0,0,width,height);
	ctx.strokeStyle = "black";
	ctx.strokeRect(0,0, width, height);
	
	move();
	
	for(var i = 0; i<p1snake_array.length; i++){
		var pos = p1snake_array[i];
		fill(pos.x, pos.y, "black");
	}
	
	for(var i =0; i<p2snake_array.length;i++){
		var pos = p2snake_array[i];
		fill(pos.x, pos.y, "blue");
	}
	
	
}

function move()
{
	//get the original pos
	var cur_p1x = p1snake_array[0].x;
	var cur_p1y = p1snake_array[0].y;
	
	var cur_p2x = p2snake_array[0].x;
	var cur_p2y = p2snake_array[0].y;
	
	//remove the last element 'the tail'
	var p1 = p1snake_array.pop();
	var p2 = p2snake_array.pop();
	
    //direction-based event (keypressed) not yet implemented
	cur_p1x++; //increment by 1 or moving right; 
	cur_p2x--; //decrememt by 1 or moving left;
	
	p1.x = cur_p1x;
	p2.x = cur_p2x;
	
	p1snake_array.unshift(p1);
	p2snake_array.unshift(p2);
}

function fill(x,y, color) //for filling a dot that can be the snake body or the food
{
	ctx.fillStyle = color;
	ctx.fillRect(x*cell_dim, y*cell_dim, cell_dim, cell_dim);
}


//FUTURE REFERENCE for keypressed
//up = 38, down = 40, right = 39, left = 37
//w  = 37, s    = 83, d     = 68, a    = 65
