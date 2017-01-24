var Server;

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
		document.getElementById("cntBtn").disabled = true;
		log("[Client] Connected.");
		sendSetPlayerIDEvent(1, document.getElementById('p1id').value);
		sendSetPlayerIDEvent(2, document.getElementById('p2id').value);
		log("[Client] IDs sent.");
	});

	// Disconnect handler
	Server.bind('close', function( data ) 
	{
		document.getElementById("cntBtn").disabled = false;
		log( "[Client] Disconnected." );
	});

	// Log any messages sent from server
	Server.bind('message', function( payload ) 
	{
		log( "[Server] " + payload );
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
	send("{\"event\": \"setPlayerIDEvent\", \"player\": " + player + ", \"id\":\"" + id + "\"}"); 
	log("[Client] Sent message: " + "{\"event\": \"setPlayerIDEvent\", \"player\": " + player + ", \"id\":\"" + id + "\"}");
}

