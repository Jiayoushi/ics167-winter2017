var Server;
var online = false;

var PLAYER_1 = 1;
var PLAYER_2 = 2;

var changedPlayer; // initialize early to save computational cycles for critical gameplay
var direction; // initialize early to save computational cycles for critical gameplay

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
		sendSetPlayerIDEvent(1, document.getElementById('pid').value);
		log("[Client] ID sent.");
	});

	// Disconnect handler
	Server.bind('close', function( data ) 
	{
		log( "[Client] Disconnected, or no server found." );
	});

	// Log any messages sent from server
	Server.bind('message', function( payload ) 
	{
		try
		{
			var theJSON = JSON.parse(payload);
			var firedEvent = theJSON.event;
			
			if (firedEvent == "playerDirectionEvent")
			{
				changedPlayer = theJSON.player;
				direction = theJSON.direction;
				
				if (changedPlayer == PLAYER_1)
				{
					if (direction == "UP")
					{
						p1_Hori = NONE;
						p1_Vert = UP;
					}
					else if (direction == "DOWN")
					{
						p1_Hori = NONE;
						p1_Vert = DOWN;
					}
					else if (direction == "LEFT")
					{
						p1_Hori = LEFT;
						p1_Vert = NONE;
					}
					else if (direction == "RIGHT")
					{
						p1_Hori = RIGHT;
						p1_Vert = NONE;
					}
				}
				else
				{
					if (direction == "UP")
					{
						p2_Hori = NONE;
						p2_Vert = UP;
					}
					else if (direction == "DOWN")
					{
						p2_Hori = NONE;
						p2_Vert = DOWN;
					}
					else if (direction == "LEFT")
					{
						p2_Hori = LEFT;
						p2_Vert = NONE;
					}
					else if (direction == "RIGHT")
					{
						p2_Hori = RIGHT;
						p2_Vert = NONE;
					}
				}
			}
			else if (firedEvent == "newRewardEvent")
			{
				var position;
				position = [{x: theJSON.X, y: theJSON.Y}] //x and y coordinate tuple
				rewards.push(position[0]); //pushes to create new reward at location in reward array
			}
			else if (firedEvent == "gameStartedEvent")
			{
				main();
			}
			else if (firedEvent == "updatePlayerNumberEvent")
			{
				playernumber = theJSON.player;
				log("[Server] You are Player #" + playernumber + ".");
				log("[Server] Your ID is " + document.getElementById('pid').value);
				if (playernumber == PLAYER_1)
				{
					log("[Client] Waiting for Player #2 to join...");
				}
				else if (playernumber == PLAYER_2)
				{
					log("[Client] Retrieving Player #1's information...");
				}
			}
			else if (firedEvent == "playerConnectedEvent")
			{
				var connectedPlayer = theJSON.player;
				if (connectedPlayer == PLAYER_1) // Player 1 has connected, only player 2 gets this event
				{
					log("[Client] Player #1 has joined the networked game.");
					log("[Client] Player #1's ID is " + theJSON.id);
					log("[Client] Waiting for Player #1 to start the game...");
				}
				else if (connectedPlayer == PLAYER_2) // Player 2 has connected, only player 1 gets this event
				{
					log("[Client] Player #2 has joined the networked game.");
					log("[Client] Player #2's ID is " + theJSON.id);
					log("[Client] Ready to start the game...");
					document.getElementById('Start').style.visibility = 'visible';
				}
			}
			else if (firedEvent == "playerDisconnectEvent")
			{
				var disconnectedPlayer = theJSON.player;
				if (disconnectedPlayer == PLAYER_1) // Player 1 has disconnected, so this client is Player 2
				{
					if (gameStarted == true || playernumber == PLAYER_2) // player #2 will never have "gameStarted" to true since it doesnt press button
					{
						clearInterval(game_interval_ID);
						dc_message(PLAYER_1);
						log("[Client] Player 1 has disconnected, so the game ended.");
						gameStarted = false;
					}
					else
					{
						log("[Client] Player 1 has disconnected.");
					}
					log("[Client] Player #1 was the leader, so the server has closed the game room and disconnected you.");
				}
				else if (disconnectedPlayer == PLAYER_2) // Player 2 has disconnected, so this client is Player 1
				{
					if (gameStarted == true)
					{
						clearInterval(game_interval_ID);
						dc_message(PLAYER_2);
						log("[Client] Player 2 has disconnected, so the game ended.");
						gameStarted = false;
					}
					else
					{
						log("[Client] Player 2 has disconnected.");
						document.getElementById('Start').style.visibility = 'hidden';
					}
					log("[Client] Waiting for Player #2 to join...");
				}
			}
		}
		catch (err)
		{
			console.log(err);
			log("[Server] " + payload);
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
}
function sendSetPlayerDirectionEvent(player, direction)
{
	send("{\"event\": \"setPlayerDirectionEvent\", \"player\": " + player + ", \"direction\":\"" + direction + "\"}");
}
function sendPlayerScoreEvent(player)
{
	send("{\"event\": \"playerScoreEvent\", \"player\": " + player + "}"); // JSON example: JSON example -> {"event": "playerScoreEvent", "player": 1}
}
function sendRewardCoordinates(x,y)
{
	send("{\"event\": \"rewardCoordinatesEvent\", \"x\": " + x + ", \"y\": " + y + "}");
}
function sendGameStartEvent()
{
	send("{\"event\": \"gameStartEvent\"" + "}"); // JSON example: JSON example -> {"event": "gameStartEvent"}
}
function sendGameFinishedEvent()
{
	send("{\"event\": \"gameFinishedEvent\"" + "}"); // JSON example: JSON example -> {"event": "gameFinishedEvent"}
}


