var gameStarted = false;

function ConnectToServer()
{
	connect();
	online = true;
	document.getElementById('NetworkConnect').style.visibility = 'hidden';
	document.getElementById('OfflineStart').style.visibility = 'hidden';
}

function Start() 
{
	sendGameStartEvent();
	gameStarted = true;
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('OfflineStart').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}

function OfflineStart()
{
	main();
	online = false;
	gameStarted = true;
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('OfflineStart').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}

function Restart()
{
	sendGameStartEvent();
	gameStarted = true;
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('OfflineStart').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}
