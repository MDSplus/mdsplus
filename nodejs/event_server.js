// GMY V20190717083800

var SSE = require('sse')
  , http = require('http');

var connections = {};
var history = {} 

var express = require('express');
var app = express();


var fs = require("fs");
var sse;


var frozenShot = -2;

//check arguments
if(process.argv.length != 3 && process.argv.length != 4)
{
    console.log('Usage: mode event_server.js <SSE Port> [debug]' );
    process.exit();
}

var debug = false;
if(process.argv.length > 3 && process.argv[3] == 'debug')
{
    debug = true;
    console.log("DEBUG enabled");
}
var eventPort;
  try {
     eventPort= parseInt(process.env['mdsevent_port']);
     if (process.env['mdsevent_port'] == undefined)
        eventPort = 4000
  }catch(err) {eventPort = 4000;}
  
console.log('Event port: ' + eventPort);

var ssePort = parseInt(process.argv[2]); 

app.get('/streams', function(req, res) {
      if(debug)console.log('REQUEST FOR NAME: '+req.query.name); 
      var data = fs.readFileSync('index.html', 'utf8');
      res.writeHead(404, {'Content-Type': 'text/html'});
      res.end(data.toString());
});

app.get('/', function(req, res) {
      if(debug)console.log('REQUEST FOR NAME: /'); 
      var data = fs.readFileSync('index.html', 'utf8');
      res.writeHead(404, {'Content-Type': 'text/html'});
      res.end(data.toString());
});

app.get('/p', function(req, res) {
      if(debug)console.log('REQUEST FOR NAME: /p'); 
      var data = fs.readFileSync('panel.html', 'utf8');
      res.writeHead(404, {'Content-Type': 'text/html'});
      res.end(data.toString());
});

app.get('/panel.html', function(req, res) {
      if(debug)console.log('REQUEST FOR NAME: /panel.html'); 
      var data = fs.readFileSync('panel.html', 'utf8');
      res.writeHead(404, {'Content-Type': 'text/html'});
      res.end(data.toString());
});

///////////////////////////////////////////////////
var pendingMessages = {};
var clients = {};
function sendMessage(message, url) {
//  listener.send('1 '+message);
  //return;
  if (pendingMessages[url] == undefined)
  {
    pendingMessages[url] = {numMessages: 0, messages: '', lastTime: Date.now()-101}; //Force the first message to be sent
  }
  pendingMessages[url].numMessages++;
  pendingMessages[url].messages += ' '+message;
 
  if(Date.now() > pendingMessages[url].lastTime + 300) //10Hz max freq update
  {
      for(var i = 0; i < clients[url].length; i++)	
          clients[url][i].send(pendingMessages[url].numMessages.toString() + pendingMessages[url].messages)
      if(debug) console.log(pendingMessages[url].numMessages.toString() + pendingMessages[url].messages)
      //console.log('Listener: ');
      //console.log(listener);
      pendingMessages[url] =  {numMessages: 0, messages: '', lastTime: Date.now()};
  }
}

//////////////////////////////////////////


function getArrayOfSignals(inputString) {
  var pathAndParams = inputString.split("?");
  var signalsString = pathAndParams[1].split("=")[1];
  var signals = signalsString.split(",");
  return signals;
}

  var server = app.listen(ssePort, function () {
   var host = server.address().address;
   var port = server.address().port;
   sse = new SSE(server);
   sse.on('connection', function(client) {
     client.on('close', function(client) { if(debug) console.log('DISCONNECT!');removeConnections(client);})
      console.log('RECEIVED CONNECTION - URL: ' + client.req.url);
      var signals = getArrayOfSignals(client.req.url);
      console.log(signals);

      if(signals != undefined)
	  handleNewConnectionList(signals, client); });
//   console.log("Example app listening at http://%s:%s", host, port)
})



function addSamples(name, shot, times, samples, isAbsoluteTime)
{
    if(debug) console.log("addSample: ", name, times, samples);
    if(history[name] == undefined || history[name].shot != shot)
    {
        history[name] = {shot: shot, isAbsoluteTime: isAbsoluteTime, times: times, samples: samples};
    }
    else
    {
        lastTime = history[name].times[history[name].times.length - 1];
	if(times[0] < lastTime)
	{
	    history[name] = {shot: shot, isAbsoluteTime: isAbsoluteTime, times: times, samples: samples};
	}
	else
	{
	    for(var i = 0; i < times.length; i++)
	    {
		history[name].times.push(times[i]);
		history[name].samples.push(samples[i]);
	    }
	}
    }
}

function handleEventReception(evMessage)
{
    try {
      var evItems = evMessage.split(' ');
      var shot = parseInt(evItems[0]);
      if(shot == frozenShot) return;
      var name = evItems[1];
      var numSamples = parseInt(evItems[3]);
      var times = [];
      var samples = [];
      for(var i = 0; i < numSamples; i++)
      {
	  times.push(parseFloat(evItems[4+i]));
	  samples.push(parseFloat(evItems[4+numSamples+i]));
      }
      if(debug) console.log('Received ' + name, numSamples);
      addSamples(name, shot, times, samples, evItems[2] == 'L');
      if(connections[name] != undefined)
      {
	  for(var i = 0; i < connections[name].listeners.length; i++)
	  {
	      if(debug) console.log('Sending '+ connections[name].timestamp.toString() + ' '+ evMessage);
	      sendMessage(connections[name].timestamp.toString() + ' '+ evMessage, connections[name].urls[i]);
	      //connections[name].listeners[i].send(connections[name].timestamp.toString() + ' '+ evMessage);
 	  }
	  connections[name].timestamp++;
      }
    }
    catch(err)
    {
        console.log("Error handling event: " + err.message);
    }
}

function removeConnections(listener)
{
    Object.keys(connections).map(x =>removeConnection(x, listener)); 
}


function removeConnection(name, listener)
{
    if(connections[name] == undefined)
	return;
    console.log(connections[name]);
    var index = connections[name].listeners.indexOf(listener);
    connections[name].listeners.splice(index, 1);
    for (client in clients)
    {
        index = client.indexOf(listener);
	if(index >= 0)
	  client.splice(index, 1);
    }
}

function handleNewConnection(name, listener)
{
  console.log('HANDLE NEW CONNECTION: '+ listener.req.url);
    if(connections[name] == undefined)
	connections[name] = {timestamp:0, listeners: [listener], urls:[listener.req.url]};
    else
    {
	connections[name].listeners.push(listener);
	connections[name].urls.push(listener.req.url);
    }
    if(history[name] != undefined)
    {
        var msg = history[name].shot.toString() + ' '+name+((history[name].isAbsoluteTime)?' L ':' F ')+ history[name].times.length.toString();
        for(var i = 0; i < history[name].times.length; i++)
	    msg = msg + ' '+ history[name].times[i].toString();
        for(var i = 0; i < history[name].samples.length; i++)
	    msg = msg + ' '+ history[name].samples[i].toString();
        if(debug) console.log("New Connection: " + msg);
        listener.send('1 '+ connections[name].timestamp.toString() + ' '+ msg);
    }
}


function handleNewConnectionList(signals, listener) {
   signals.map(x => handleNewConnection(x, listener));
   
    if(clients[listener.req.url] == undefined)
        clients[listener.req.url] = [listener];
    else
        clients[listener.req.url].push(listener);
}


function handleFreezeShot(shotStr)
{
    console.log('Freeze shot '+shotStr);
    frozenShot = parseInt(shotStr);
}

function handleUnfreezeShot()
{
    console.log('Unfreeze shot '+ frozenShot);
    frozenShot = -2;
}

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
  
//  console.log('Event from ' + rinfo.address+':'+rinfo.port+'   '+len + '   '+ eventName + '  '+dataLen);
//  if(dataLen > 0)
//      console.log(eventData);
  
  if(eventName == 'STREAMING')
  {
      handleEventReception(eventData);
  }
  else if(eventName == 'STREAMING_FREEZE')
  {
      handleFreezeShot(eventData);
  }
  else if (eventName = 'STREAMING_UNFREEZE')
  {
      handleUnfreezeShot()
  }
}


eventServer.bind(eventPort, function(){
    eventServer.setBroadcast(true);
    eventServer.setMulticastTTL(128);
    var multicastAddress;
    try {
	multicastAddress = process.env['mdsevent_address'];
	if (multicastAddress != undefined)
	{
	  eventServer.addMembership(multicastAddress);
	  console.log('Multicast Address: ' + multicastAddress);
	}
	else
	{
	  eventServer.addMembership("224.0.0.175");
	  eventServer.addMembership("225.0.0.175");
	}
	
    }catch(err)
    {
	eventServer.addMembership("224.0.0.175");
	eventServer.addMembership("225.0.0.175");
    }
});


