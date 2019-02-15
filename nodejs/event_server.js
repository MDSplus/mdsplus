var SSE = require('sse')
  , http = require('http');

var connections = {};
var history = {} 

var express = require('express');
var app = express();


var fs = require("fs");
var sse;

var pendingName; //Name of pending name, valid from the extablishment of a new /stream connection and the activation of the corresponding SSE

app.get('/streams', function(req, res) {
      console.log('REQUEST FOR NAME: '+req.query.name); 
      pendingName = req.query.name;
      var data = fs.readFileSync('sample_display.html', 'utf8');
      //console.log(data);
      res.writeHead(404, {'Content-Type': 'text/html'});
      res.end(data.toString());
});
 
var port = parseInt(process.argv[2])
console.log(process.argv[2])

var server = app.listen(port, function () {
//var server = app.listen(8082, function () {
   var host = server.address().address
   var port = server.address().port
   sse = new SSE(server);
   sse.on('connection', function(client) {
      console.log('RECEIVED CONNECTION for '+pendingName);
      if(pendingName != undefined)
	  handleNewConnection(pendingName, client); });
      pendingName = undefined;
   console.log("Example app listening at http://%s:%s", host, port)
})



function addSamples(name, shot, times, samples, isAbsoluteTime)
{
    if(history[name] == undefined || history[name].shot != shot)
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

function handleEventReception(evMessage)
{
    try {
      var evItems = evMessage.split(' ');
      var shot = parseInt(evItems[0]);
      var name = evItems[1];
      var numSamples = parseInt(evItems[3]);
      var times = [];
      var samples = [];
      for(var i = 0; i < numSamples; i++)
      {
	  times.push(parseFloat(evItems[4+i]));
	  samples.push(parseFloat(evItems[4+numSamples+i]));
      }
      addSamples(name, shot, times, samples, evItems[2] == 'L');
      if(connections[name] != undefined)
      {
	  for(var i = 0; i < connections[name].listeners.length; i++)
	  {
	      console.log('Spedisco '+ connections[name].timestamp.toString() + ' '+ evMessage);
	      connections[name].listeners[i].send(connections[name].timestamp.toString() + ' '+ evMessage);
	  }
	  connections[name].timestamp++;
      }
    }
    catch(err)
    {
        console.log("Error handling event: " + err.message);
    }
}

function handleNewConnection(name, listener)
{
    if(connections[name] == undefined)
	connections[name] = {timestamp:0, listeners: [listener]};
    else
	connections[name].listeners.push(listener);
    if(history[name] != undefined)
    {
        var msg = history[name].shot.toString() + ' '+name+((history[name].isAbsoluteTime)?' L ':' F ')+ history[name].times.length.toString();
        for(var i = 0; i < history[name].times.length; i++)
	    msg = msg + ' '+ history[name].times[i].toString();
        for(var i = 0; i < history[name].samples.length; i++)
	    msg = msg + ' '+ history[name].samples[i].toString();
        console.log("New Connection: " + msg);
        listener.send(connections[name].timestamp.toString() + ' '+ msg);
    }
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
  
  console.log('Event from ' + rinfo.address+':'+rinfo.port+'   '+len + '   '+ eventName + '  '+dataLen);
  if(dataLen > 0)
      console.log(eventData);
  
  if(eventName == 'STREAMING')
  {
      handleEventReception(eventData);
  }
}


eventServer.bind(4000, function(){
    eventServer.setBroadcast(true);
    eventServer.setMulticastTTL(128);
	eventServer.addMembership("224.0.0.175");
	eventServer.addMembership("225.0.0.175");
});


