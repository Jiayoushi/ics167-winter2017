var Server;
var online = false;

var PLAYER_1 = 1;
var PLAYER_2 = 2;

var changedPlayer; // initialize early to save computational cycles for critical gameplay
var direction; // initialize early to save computational cycles for critical gameplay

var latencyLoopInterval = 500; // in milliseconds
var latencyMessages = []; // 2^32 possible elements
var latencyMessageID = 0;
var latencyLoopID = -1;

var initialTimestamp;
var EstimatedRTT=0;

var rwd_buffer = [];

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
		initialTimestamp = Date.now();
		latencyLoopID = setInterval(doLatencyEstimation, latencyLoopInterval);
		log("[Client] Registered latency estimation loop (" + latencyLoopInterval + " ms).");
	});

	// Disconnect handler
	Server.bind('close', function( data ) 
	{
		log( "[Client] Disconnected, or no server found." );
		clearInterval(latencyLoopID);
		latencyMessages = [];
		latencyMessageID = 0;
	});

	// Log any messages sent from server
	Server.bind('message', function( payload ) 
	{
		try
		{
			var theJSON = JSON.parse(payload);
			var firedEvent = theJSON.event;
			
			if (firedEvent == "loopEvent" && gameStarted)
			{
			        update_snake(theJSON.body1, theJSON.body2);	
		
            		}
            		else if(firedEvent == "playerDirectionEvent")
            		{
                		processDirection(theJSON.player, theJSON.direction);
            		}
		    	else if(firedEvent == "playerScoreRelayEvent" && gameStarted)
			{
				if(theJSON.player == PLAYER_1)
				{
					p1_score++;
					text_Ctx.fillStyle = 'white'; 		// White out old score text.
	   				text_Ctx.fillRect(p1_scoretext_x, scoretext_y-10,30,10);
	    			text_Ctx.fillStyle = 'black'; 		// Write in new score.
	    			text_Ctx.fillText(p1_score, p1_scoretext_x, scoretext_y);
				}
				else
				{
					p2_score++;
					text_Ctx.fillStyle = 'white'; 		// White out old score text.
    				text_Ctx.fillRect(p2_scoretext_x, scoretext_y-10,30,10);
	    			text_Ctx.fillStyle = 'black'; 		// Write in new score.
	    			text_Ctx.fillText(p2_score, p2_scoretext_x, scoretext_y)
				}
			}
			else if (firedEvent == "newRewardEvent")
			{               
                if (!gameStarted)
                {
                    rwd_buffer.push({x:theJSON.new_x, y:theJSON.new_y});
                }
                else
                {
                    if (theJSON.del_x != -1)
                    {
                        delete_reward(theJSON.del_x, theJSON.del_y);
                    }              
                    rewards.push({x:theJSON.new_x, y:theJSON.new_y});
                }
            }
			else if (firedEvent == "gameStartedEvent")
			{
                log("[Client] Game Started.");
				gameStarted = true;
				main();
				if(playernumber == PLAYER_1)
					document.getElementById('Restart').style.visibility = 'hidden';
			}
			else if (firedEvent == "gameFinishedEvent")
			{
				log("[Server] GameFinishedEvent");
				
				clearInterval(game_interval_ID);
				win_message(theJSON.winner);
				gameStarted = false;
                rwd_buffer = [];
                frame = 0;
				if(playernumber==1)
					document.getElementById('Restart').style.visibility = 'visible';
			}
			else if (firedEvent == "latencyEstimationEvent")
			{
				handleLatencyEstimation(theJSON.id, theJSON.X, theJSON.Y);
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
					}
					document.getElementById('Restart').style.visibility = 'hidden';
					document.getElementById('Start').style.visibility = 'hidden';
					log("[Client] Waiting for Player #2 to join...");
				}
			}
		}
		catch (err)
		{
			log("[Server] " + payload); // <- If a JSON error occurs, or no event matched, then it prints out the message. Use to your advantage!
			//console.log(err); //<- Since this is a try, catch, if you see your event being printed in raw JSON, that means there's an error. Try looking at the err.
		}
	});
	
	Server.connect();
}

function send( text ) 
{
	Server.send( 'message', text );
}

function doLatencyEstimation()
{
	sendLatencyEstimationEvent(latencyMessageID++, Date.now() - initialTimestamp);
}

function handleLatencyEstimation(id, X, Y)
{
	var B = Date.now() - initialTimestamp;
    var SampleRTT = B - latencyMessages[id] - (Y-X);
    EstimatedRTT = Math.round(0.875*EstimatedRTT + 0.125*SampleRTT);
    document.getElementById('RTT').value = EstimatedRTT + "ms";
	//log("[Debug] Latency ID: " + id + " | A: " + latencyMessages[id] + " | X: " + X + " | Y: " + Y + " | B: " + B);
	//log("[Client] Latency Estimation (ID #" + id + "): " + EstimatedRTT + " ms.");
	// A: Timestamp of Client when he sent the packet.
	// B: Timestamp of Client when he received the reply.
	// X: Timestamp of Server when he received the packet.
	// Y: Timestamp of Server when he sent the reply.
	
	// Reason for this terminology: http://stackoverflow.com/questions/1228089/how-does-the-network-time-protocol-work
}

function processDirection(changedPlayer, direction)
{
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

/* Begin Custom Functions */
function sendSetPlayerIDEvent(player, id)
{
	send("{\"event\": \"setPlayerIDEvent\", \"player\": " + player + ", \"id\":\"" + id + "\"}"); // JSON example: JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
}
function sendSetPlayerDirectionEvent(player, direction)
{
	send("{\"event\": \"setPlayerDirectionEvent\", \"player\": " + player + ", \"direction\":\"" + direction + "\"}");
}
function sendPlayerScoreEvent(player, index)
{
	send("{\"event\": \"playerScoreEvent\", \"player\": " + player + ", \"index\": " + index + "}");
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

function sendLatencyEstimationEvent(id, timestamp)
{
	send("{\"event\": \"latencyEstimationEvent\", \"id\": " + id + "}");
	latencyMessages.push(timestamp);
}


