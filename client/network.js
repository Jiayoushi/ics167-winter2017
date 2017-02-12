var Server;
var online = false;

/* Begin TA Functions */
function log( text ) 
{
	$log = $('#log');
	//Add text to log
	$log.append(($log.val()?"\n":'')+text);
	//Autoscroll
	$log[0].scrollTop = $log[0].scrollHeight - $log[0].clientHeight;
}

function connect()
{
	log('[Client] Connecting...');
	Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value);

	// Input handler
	$('#message').keypress(function(e) {
		if ( e.keyCode == 13 && this.value ) {
			log( '[Client] ' + this.value );
			send( this.value );
	
			$(this).val('');
		}
	});

	// Connected handler
	Server.bind('open', function() 
	{
		log("[Client] Connected.");
		sendSetPlayerIDEvent(1, document.getElementById('p1id').value);
		//sendSetPlayerIDEvent(2, document.getElementById('p2id').value); //Connect one at a time
		log("[Client] IDs sent.");
	});

	// Disconnect handler
	Server.bind('close', function( data ) 
	{
		log( "[Client] Disconnected, or no server found." );
	});

	// Log any messages sent from server
	Server.bind('message', function( payload ) 
	{
		log( "[Server] " + payload );
		if(payload=="Both clients have now been connected.")
		{
			document.getElementById('Start').style.visibility = 'visible';
		}
		if(payload.split("-")[0] == "RewardCoordinates")
		{
			log(payload.split("-")[1]+"-"+payload.split("-")[2]);
			var position;
			position = [{x:Number(payload.split("-")[1]),
                       y:Number(payload.split("-")[2])}]; 
			rewards.push(position[0]);
		}
		if(payload == "Player#1")
		{
			playernumber = 1;
		}
		if(payload == "Player#2")
		{
			playernumber = 2;
		}
		if(payload == "P1 Direction: UP")
		{
			p1_Hori = NONE;
            p1_Vert = UP;
		}
		if(payload == "P1 Direction: DOWN")
		{
			p1_Hori = NONE;
            p1_Vert = DOWN;
		}
		if(payload == "P1 Direction: LEFT")
		{
			p1_Hori = LEFT;
            p1_Vert = NONE;
		}
		if(payload == "P1 Direction: RIGHT")
		{
			p1_Hori = RIGHT;
            p1_Vert = NONE;
		}
		if(payload == "P2 Direction: UP")
		{
			p2_Hori = NONE;
            p2_Vert = UP;
		}
		if(payload == "P2 Direction: DOWN")
		{
			p2_Hori = NONE;
            p2_Vert = DOWN;
		}
		if(payload == "P2 Direction: LEFT")
		{
			p2_Hori = LEFT;
            p2_Vert = NONE;
		}
		if(payload == "P2 Direction: RIGHT")
		{
			p2_Hori = RIGHT;
            p2_Vert = NONE;
		}
		if(payload == "Game has been started!")
		{
			main();
		}
	});

	Server.connect();
}

function send( text ) 
{
	Server.send( 'message', text );
}

/* Begin Custom Functions */
function sendSetPlayerIDEvent(player, id)
{
	send("{\"event\": \"setPlayerIDEvent\", \"player\": " + player + ", \"id\":\"" + id + "\"}"); // JSON example: JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
	//log("[Client] Sent message: " + "{\"event\": \"setPlayerIDEvent\", \"player\": " + player + ", \"id\":\"" + id + "\"}");
}
function sendSetPlayerDirectionEvent(player, direction)
{
	send("{\"event\": \"setPlayerDirectionEvent\", \"player\": " + player + ", \"direction\":\"" + direction + "\"}");
}
function sendPlayerScoreEvent(player)
{
	send("{\"event\": \"playerScoreEvent\", \"player\": " + player + "}"); // JSON example: JSON example -> {"event": "playerScoreEvent", "player": 1}
	//log("[Client] Sent message: " + "{\"event\": \"playerScoreEvent\", \"player\": " + player + "}");
}
function sendRewardCoordinates(x,y)
{
	send("{\"event\": \"rewardCoordinatesEvent\", \"x\": " + x + ", \"y\": " + y + "}");
}
function sendGameStartEvent()
{
	send("{\"event\": \"gameStartEvent\"" + "}"); // JSON example: JSON example -> {"event": "gameStartEvent"}
	//log("[Client] Sent message: " + "{\"event\": \"gameStartEvent\"" + "}");
}
function sendGameFinishedEvent()
{
	send("{\"event\": \"gameFinishedEvent\"" + "}"); // JSON example: JSON example -> {"event": "gameFinishedEvent"}
	//log("[Client] Sent message: " + "{\"event\": \"gameFinishedEvent\"" + "}");
}


