function Start() 
{
	connect();
	online = true;
	main();
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('OfflineStart').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}

function OfflineStart()
{
	main();
	online = false;
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('OfflineStart').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}

function Restart()
{
	main();
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('OfflineStart').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}
