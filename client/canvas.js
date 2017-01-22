var canvas;
var context;
var board;

const ROW_UNIT = 15;
const COLUMN_UNIT = 15;
var rows = 50;
var columns = 50;

var EMPTY = 0;
var SNAKE = 1;
var ITEM = 2;

function test() {
    //document.write("test()\n");
     
    //Test board
    for(var i=0;i<columns;i++){
        for(var k=0;k<rows;k++){
            if(typeof(board.grid[i][k]) != typeof([]) ) {
                return "Test failed";
            }
        }
    }
   
    return "Test Passed";  
}

class Board {
   
    constructor(_columns, _rows) {
        this.columns = _columns;
        this.rows = _rows;
        this.grid = [];    
    
        for(var i=0; i<this.columns; i++){
            this.grid.push([]);
            for(var k=0; k<this.rows; k++){
                this.grid[i].push([]);
            }
        }
    }
}

class snake {

    constructor(_x,_y) {
        this.x = _x;
        this.y = _y;
    }
}

function init_objects() {
    board = new Board(columns,rows);
}

function init_canvas()  {

    // Create a canvas
    canvas = document.createElement("canvas");
    canvas.width = columns*COLUMN_UNIT;
    canvas.height = rows*ROW_UNIT;
    context = canvas.getContext("2d");

    // add the canvas element to the body of the document
    document.body.appendChild(canvas);
}

function init() {
    init_objects();
    init_canvas();
}

function main() {
    //document.write("main()\n");
    init();   
}
