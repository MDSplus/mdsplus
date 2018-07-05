//
// URI to call the code:
//
// 1 - http://localhost:8081/connect?ip=<ipAddress[:port]>  
// return: connection ID to be used in the following transactions
// 2 - http://localhost:8081/eval?expr=<expression>&idx=<connection ID>
//
// server: $ node mdsipRest-multi.js
//

// OLD http://localhost:8081/eval?expr=thestring
// OLD http://localhost:8081/eval?expr=2%2B3.14  (%2B = '+') https://www.url-encode-decode.com/


var net = require('net');
var clients = {};
var incompleteData = {};
var pendingDataLen = {};
var currentData = {};
var initialized = {};
var msgId = {};
var connectionCount = 1;
var ress = {};
var ips = {};
var wsConnections=[];
var registeredEvents = [];

var openMdsip = function(ip, res)
{
	var ipv = ip.split(':');
	var ipAddr = ipv[0];
	var port;
	if(ipv.length > 1)
		port = parseInt(ipv[1]);
	else
		port = 8000;
	ips[connectionCount] = ip;
	ress[connectionCount] = res;
	ress[connectionCount] = res;
	var currConnIdx = connectionCount;
	var client = net.connect(port, ipAddr, function() {
		console.log('connected to server!');  
		client.on('data', (function (inConnIdx)
			{ var connIdx = inConnIdx;
			console.log('CONN IDX = ', connIdx);
			  return function(data) {handleData(data, connIdx);};
			})(currConnIdx)); 
	});
	var buf = new Buffer(57);
	buf.writeInt32BE(57, 0);
	buf.writeInt32BE(0, 4);
	buf.writeInt16BE(9, 8);
	buf.writeUInt8(1, 10);
	buf.writeUInt8(0, 11);
	buf.writeUInt8(1, 12);
	buf.writeUInt8(14, 13);
	buf.writeUInt8(0x80|0x40|0x3, 14);
	buf.writeUInt8(0, 15);
	buf.writeInt32BE(0, 16);
	buf.writeInt32BE(0, 20);
	buf.writeInt32BE(0, 24);
	buf.writeInt32BE(0, 28);
	buf.writeInt32BE(0, 32);
	buf.writeInt32BE(0, 36);
	buf.writeInt32BE(0, 40);
	buf.writeInt32BE(0, 44);
	buf.write('JAVA_USER', 48);
	client.write(buf); 
	clients[connectionCount] = client;
	msgId[connectionCount] = 1;
	connectionCount++;
}




var sendMdsipReq = function(expr, connIdx)
 {
	client = clients[connIdx];
	if(client == undefined)
	{
		ress[connIdx].end('server '+ips[connIdx]+'  not connected');
		return;
	}
	var buf = new Buffer(48+expr.length);
	buf.writeInt32BE(48+expr.length, 0);
	buf.writeInt32BE(0, 4);
	buf.writeInt16BE(expr.length, 8);
	buf.writeUInt8(1, 10);
	buf.writeUInt8(0, 11);
	buf.writeUInt8(msgId[connIdx], 12);
//	buf.writeUInt8(2, 12);
	buf.writeUInt8(14, 13);
	buf.writeUInt8(0x80|0x40|0x3, 14);
	buf.writeUInt8(0, 15);
	buf.writeInt32BE(0, 16);
	buf.writeInt32BE(0, 20);
	buf.writeInt32BE(0, 24);
	buf.writeInt32BE(0, 28);
	buf.writeInt32BE(0, 32);
	buf.writeInt32BE(0, 36);
	buf.writeInt32BE(0, 40);
	buf.writeInt32BE(0, 44);
	buf.write(expr, 48);
	client.write(buf); 
	msgId[connIdx]++;
 }

var handleData = function(data, connIdx){
	 //console.log(data.toJSON());
	 //console.log(data.toString('ascii', 48));
	 
	var fullData;
	if(initialized[connIdx] == undefined)  //This is the answer to the first message launched once when the channel has been created (POSSIBLE BUG: the message may be split)
	{
		initialized[connIdx] = 1;
		ress[connIdx].end(connIdx.toString());
		return;
	}
	if(incompleteData[connIdx] == undefined)
	{
		 incompleteData[connIdx] = false;
		 pendingDataLen[connIdx] = 0;
	}
	if(incompleteData[connIdx])  //If it is true we have already received part of the return expr and currentData has already been defined
	{
		currentData[connIdx] = Buffer.concat([currentData[connIdx], data]);
			//console.log('Pending data len: ', pendingDataLen[connIdx]);
			//console.log('Current data len: ', currentData[connIdx].length);
		if(pendingDataLen[connIdx] > currentData[connIdx].length) //Still missing data
		{
			//console.log('INCOMPLETE DATA');
			return;
		}
		else
		{
			incompleteData[connIdx] = false;
			pendingDataLen[connIdx] = 0;
			fullData = currentData[connIdx]; //Whole message receive, proceed
		}
	}
	else  //!incompleteData means that it is either the full message or the first part of it 
	{
		var msgLen = data.readInt32BE(0);
		if(msgLen > data.length) //This is the first part of an incomplete data message
		{
			incompleteData[connIdx] = true;
			currentData[connIdx] = data;
			//currentData[connIdx] = fullData;
			pendingDataLen[connIdx] = msgLen;
			return;
		}
		fullData = data;
	}
	//If we arrive here, fullData contains the whole message 
	var answLen = fullData.readInt16BE(8);
	var dtype = fullData.readInt8(13)
	var nDims  = fullData.readInt8(15);
	console.log('msglen: ', msgLen);
	console.log('Answer len: ', answLen);
	console.log('dtype: ', dtype);
	console.log('nDims: ', nDims);
	var totDim = 1;
	var decompiled = '';
	for(var i = 0; i < nDims; i++)
	{
		decompled = decompiled + '[';
		var currDim = fullData.readInt32BE(16+4*i);
		totDim = totDim * currDim;
		//console.log('Dim: ', currDim);
	}
	if(nDims > 0)
		decompiled += '[';
	for(var i = 0; i < totDim; i++)
	{
		switch(dtype) {
			case 2: decompiled += fullData.readUInt8(48+i).toString(); break;
			case 3: decompiled += fullData.readUInt16BE(48+2*i).toString(); break;
			case 4: decompiled += fullData.readUInt32BE(48+4*i).toString(); break;
			case 5: decompiled += fullData.readUInt64BE(48+8*i).toString(); break;
			case 6: decompiled += fullData.readInt8BE(48+i).toString(); break;
			case 7: decompiled += fullData.readInt16BE(48+2*i).toString(); break;
			case 8: decompiled += fullData.readInt32BE(48+4*i).toString(); break;
			case 9: decompiled += fullData.readInt64BE(48+8*i).toString(); break;
			case 10: decompiled += fullData.readFloatBE(48+4*i).toString(); break;
			case 11: decompiled += fullData.readDoubleBE(48+8*i).toString(); break;
//			case 14: decompiled += "'"+fullData.slice(48,48+answLen).toString()+"'"; break;
			case 14: decompiled += "'"+fullData.slice(48+answLen*i,48+answLen*(i+1)).toString()+"'"; break;
		}
		if(i < totDim - 1)
			decompiled +=',';
	}
	if(nDims > 0)
		 decompiled += ']';
	 //console.log("{'reply': " + decompiled + "}");
	 //extRes.end("{'reply': " + decompiled + "}");
	console.log(decompiled);
	ress[connIdx].end(decompiled);

};

var express = require('express');
var app = express();

app.get('/eval', function (req, res) {
	console.log('Expression: ', req.query['expr'], 'ip: ', req.query['ip']);
	var expr = req.query['expr']
	var connIdx = req.query['idx']
	ress[connIdx] = res;
	sendMdsipReq(expr, connIdx);
});

app.get('/connect', function (req, res) {
	console.log('Connect: ', 'ip: ', req.query['ip']);
	var ip = req.query['ip']
	openMdsip(ip, res);
})

app.get('/register', function (req, res) {
	console.log('Register for event: ', req.query['event']);
	registeredEvents[registeredEvents.length] = req.query['event'];
	res.end('Event '+req.query['event']+' registered');
})

var server = app.listen(8081, function () {

	//var host = server.address().address
	//var port = server.address().port

	var host = "localhost"
	var port = 8081


})





////////////////EVENT STUFF////////////////////
const dgram = require('dgram');
const eventServer = dgram.createSocket({
	type: 'udp4',
	reuseAddr: true
});

eventServer.on('error', (err) => {
  console.log('eventServer error:'+err.stack);
  eventServer.close();
});

eventServer.on('message', handleEvent);

eventServer.on('listening', () => {
  const address = eventServer.address();
  console.log('eventServer listening at ' + address.address+':'+address.port);
});

function handleEvent(msg, rinfo)
{
  var len=msg.readInt32BE(0);
  var dataLen = msg.readInt32BE(4+len)
  var eventName =  msg.toString('ascii', 4,4+len);
  var eventData = msg.toString('ascii', 4+len+4);
  
  console.log('Event from ' + rinfo.address+':'+rinfo.port+'   '+len + '   '+ eventName + '  '+dataLen);
  if(dataLen > 0)
	  console.log(eventData);
  for(var i = 0; i < registeredEvents.length; i++)
  {
	  if(registeredEvents[i] == eventName)
	  {
		  handleRegisteredEvent(eventName, eventData);
		  return;
	  }
  }
}

function handleRegisteredEvent(eventName, eventData)
{
//	console.log('RegisteredEvent: '+eventName);
	if(wsConnections.length > 0)
	{
		for(var i = 0; i < wsConnections.length; i++)
		{
			if(wsConnections[i] != undefined)
				wsConnections[i].sendUTF("Event:"+eventName+';Data:'+eventData);
		}
	}
}

function getMulticastLastIp(eventName)
{
	var hash = 0;
	for (var i = 0; i < eventName.length; i++)
		hash += eventName.charCodeAt(i);
    var hashnew = Math.floor(((hash % 256)/256.) * 176.)
	return hashnew;
}


eventServer.bind(4000, function(){
    eventServer.setBroadcast(true);
    eventServer.setMulticastTTL(128);
	eventServer.addMembership("224.0.0.175");
	eventServer.addMembership("225.0.0.175");
});

/////////////////////////////////////////
////////WebSocket Stuff/////////////////
var WebSocketServer = require('websocket').server;

// create the server
var wsServer = new WebSocketServer({
  httpServer: server
});

// WebSocket server
wsServer.on('request', function(request) {
		console.log('Received WebSocket connection');
		var connection= request.accept(null, request.origin);
		wsConnections[wsConnections.length] = connection;
  // This is the most important callback for us, we'll handle
  // all messages from users here.
		connection.on('message', function(message) {
			if (message.type === 'utf8') {
			{
			//WebSocket is used ONLY to send receivedevents
			}
		}
		});

		connection.on('close', function(connection) {
			console.log('WebSocked disconnected');
			for(var i = 0; i < wsConnections.length; i++)
			{
//				console.log(wsConnections[i], connection);
				if(wsConnections[i] == connection)
				{
					console.log('DELETE MATCH');
					wsConnections[i] = undefined;
				}
			}
		});
});



