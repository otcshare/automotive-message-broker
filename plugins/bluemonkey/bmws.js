/// This is the js api for bluemonkey websockets

function listProperties(obj) {
   var propList = "";
   for(var propName in obj) {
	  propList += (propName + ", ");
   }
   console.log(propList);
}

function _hookupWebSocket(socket)
{
	socket.client.connected.connect(function()
	{
		if(socket.onopen !== undefined)
			socket.onopen();
		else
			console.log('onopen is like, totally undefined');
	});

	socket.client.disconnected.connect(function()
	{
		if(socket.onclose !== undefined)
			socket.onclose()
	});

	socket.client.error.connect(function(err)
	{
		if(socket.onerror !== undefined)
			socket.onerror(socket.client.errorString());
	});

	socket.client.textMessageReceived.connect(function(msg)
	{
		if(socket.onmessage !== undefined)
		{
			socket.onmessage(msg);
		}
	});

	socket.client.binaryMessageReceived.connect(function(msg)
	{
		if(socket.onmessage !== undefined)
		{
			socket.onmessage(msg);
		}
	});
}

function WebSocket(url, protocols, client)
{
	if(client === undefined)
	{
		this.client = websockets.createClient(url, protocols);
	}
	else
	{
		this.client = client
		_hookupWebSocket(this);
	}
	Object.defineProperty(this, 'binaryType', { enumerable : false, get : function() { return this.client.binaryType; }, set : function (val)
	{
		this.client.binaryType = val;
	} });
	Object.defineProperty(this, 'bufferedAmount', { value : 0 });
	Object.defineProperty(this, 'extensions', { value : ''});
	Object.defineProperty(this, 'protocol', { value : ''});
	Object.defineProperty(this, 'readyState', { enumberable : false, get : function ()
	{
		ready = this.client.readyState;
		if(ready == -1)
			return undefined;
		return ready;
	} });
	Object.defineProperty(this, 'url', { get : function() { return this.client.url; }});
}

WebSocket.prototype.open = function()
{
	_hookupWebSocket(this)
	this.client.open();
}

WebSocket.prototype.send = function(msg)
{
	this.client.send(msg);
};

WebSocket.prototype.close = function() { this.client.close(); };

function WebSocketServer(useSsl)
{
	if(useSsl === undefined)
		useSsl = false;
	this.server = websockets.createServer(useSsl);
	this._connectAllTheThings(this);
}

WebSocketServer.prototype._connectAllTheThings = function(obj)
{
	obj.server.newConnection.connect(function(c)
	{
		if(obj.onconnection !== undefined)
			obj.onconnection(new WebSocket('', '', c));
		else
			console.log("onconnection is not defined for WebSocketServer");
	});
};

WebSocketServer.prototype.listen = function(port)
{
	this.server.listen(port);
};

WebSocketServer.prototype.close = function(){ this.server.close(); };
