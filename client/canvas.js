var COLS = 30;
var ROWS = 30;
var canvas;
var context;

function initialize_canvas() 
{
    // create and initiate the canvas element
    canvas = document.createElement("canvas");
    canvas.width = COLS*15;
    canvas.height = ROWS*15;
    ctx = canvas.getContext("2d");
    // add the canvas element to the body of the document
    document.body.appendChild(canvas);
}