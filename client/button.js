var gameStarted = false;

function ConnectToServer()
{
	connect();
	online = true;
	document.getElementById('NetworkConnect').style.visibility = 'hidden';
}

function Start() 
{
	sendGameStartEvent();
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}

function Restart()
{
	sendGameStartEvent();	
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}
