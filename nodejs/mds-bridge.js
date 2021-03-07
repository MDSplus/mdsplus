var net = require('net');
var WebSocketServer = require('websocket').server;
var http = require('http');

//var mdsipNet = require('net');
var clients = {};
var incompleteData = {};
var pendingDataLen = {};
var currentData = {};
var initialized = {};
var msgId = {};
var connectionCount = 1;
var resolves = {};
var rejects = {}
var connectResolves = {};
var connectRejects = {};
var openTrees = {};
var openShots = {};
var lastExpressions = {};
var ips = {};
var experiments = {};
var shots = {};
var wsConnections=[];
var busy = {};
var pendingReqs = {};
var registeredStreams = [];
var clientIds = {};


var webSocketPort = 8020;
var debug = 1;
var refreshRate = 2000;
var fs = require('fs');
try {
    var configJson = fs.readFileSync('mds-bridge.json');
    var configObj = JSON.parse(configJson.toString());
    console.log(configObj);
    debug = configObj.debug;
    webSocketPort = configObj.port;
    refreshRate = configObj.refresh;
}catch(error){} //keep defaults
    


var W3CWebSocket = require('websocket').w3cwebsocket;
//var WebSocketClient = require('websocket').client;
var webSocketConnection;

var openMdsip = function(ip)
//var openMdsip = function(ip) //Nodejs
{
	var ipv = ip.split(':');
	var ipAddr = ipv[0];
	var port;
	if(ipv.length > 1)
		port = parseInt(ipv[1]);
	else
		port = 8000;
	ips[connectionCount] = ip;
	var currConnIdx = connectionCount;
    var client = net.connect(port, ipAddr, function() {
        if(debug >= 1) console.log('connected to server!'); 
        initialized[currConnIdx] = false;
        sendHeader(client);
    });       
    client.on('data', (function (inConnIdx)
    { var connIdx = inConnIdx;
      return function(message) 
      {
        handleData( message, connIdx);
      }
    })(currConnIdx));
    client.on('error', function(error) {
        console.log(error);
        connectRejects[currConnIdx](''+error);
    });
 	clients[connectionCount] = client;
	msgId[connectionCount] = 1;
	connectionCount++;
    busy[currConnIdx] = false;
    pendingReqs[currConnIdx]= [];
    return new Promise((resolve, reject)=>{ connectResolves[currConnIdx] = resolve; connectRejects[currConnIdx] = reject});
 }  
    
   // //localhost:8080/', 'echo-protocol');
function sendHeader(connection)
{
//var buf = new Buffer(57);
    var buf = Buffer.alloc(57);
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
    connection.write(buf); 
    //console.log('SENT HEADER');
}

var sendMdsipReq = function(expr, connIdx)
 {
	var client = clients[connIdx];
	if(client == undefined)
	{
            console.log('server '+ips[connIdx]+'  not connected');
            rejects[connIdx]('Not Connected');
		return;
	}
	//var buf = new Buffer(48+expr.length);
	var buf = Buffer.alloc(48+expr.length);
	buf.writeInt32BE(48+expr.length, 0);
	buf.writeInt32BE(0, 4);
	buf.writeInt16BE(expr.length, 8);
	buf.writeUInt8(1, 10);
	buf.writeUInt8(0, 11);
	buf.writeUInt8(msgId[connIdx]%256, 12);
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
    return new Promise((resolve, reject) => {
        resolves[connIdx] = resolve;
        rejects[connIdx] =  reject;
    });
 }

var handleData = function(data, connIdx){
	var fullData;
	if(initialized[connIdx] == undefined || initialized[connIdx] == false)  //This is the answer to the first message launched once when the channel has been created (POSSIBLE BUG: the message may be split)
	{
            console.log("MdsIp connected");
            initialized[connIdx] = true;
            connectResolves[connIdx](connIdx);  //ONLY AT THIS POINT CONNECTION IS COMPLETE!!!
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
		if(pendingDataLen[connIdx] > currentData[connIdx].length) //Still missing data
		{
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
	//console.log('msglen: ', msgLen);
	//console.log('Answer len: ', answLen);
	//console.log('dtype: ', dtype);
	//console.log('nDims: ', nDims);
	var totDim = 1;
	var decompiled = '';
	for(var i = 0; i < nDims; i++)
	{
		var currDim = fullData.readInt32BE(16+4*i);
		totDim = totDim * currDim;
		//console.log('Dim: ', currDim);
	}
    
    var result = [];
	for(var i = 0; i < totDim; i++)
	{
		switch(dtype) {
			case 2: result.push(fullData.readUInt8(48+i)); break;
			case 3: result.push(fullData.readUInt16BE(48+2*i)); break;
			case 4: result.push(fullData.readUInt32BE(48+4*i)); break;
			case 5: result.push(fullData.readUInt32BE(48+8*i) << 32 + fullData.readUInt32BE(48+8*i+4)); break;
//			case 5: result.push(fullData.readUInt64BE(48+8*i)); break;
			case 6: result.push(fullData.readInt8BE(48+i)); break;
			case 7: result.push(fullData.readInt16BE(48+2*i)); break;
			case 8: result.push(fullData.readInt32BE(48+4*i)); break;
			case 9: 
                var highVal = fullData.readUInt32BE(48+8*i);
                var lowVal = fullData.readUInt32BE(48+8*i+4);
                var highVal1 =  highVal * 2**32
                var val64 = highVal1 + lowVal;
                result.push(val64);
                break;
			//case 9: result.push(fullData.readUInt32BE(48+8*i) << 32 + fullData.readUInt32BE(48+8*i+4)); break;
//			case 9: result.push(fullData.readInt64BE(48+8*i)); break;
			case 10: result.push(fullData.readFloatBE(48+4*i)); break;
			case 11: result.push(fullData.readDoubleBE(48+8*i)); break;
			case 14: result.push("'"+fullData.slice(48+answLen*i,48+answLen*(i+1)).toString()+"'"); break;
		}
    }
	resolves[connIdx](result);
};

var connIdx;
var testMsg = function(connIdx) { /*console.log(clients[connIdx]); */ sendMdsipReq('[2:20:1]+3', connIdx).then((decompiled) = (decompiled) => {console.log(decompiled);});};
var testIp = function(connIdx) { sendMdsipReq('treeopen("tutorial", 1)', connIdx).then((status) => {console.log(status);  sendMdsipReq('trends:trend_1', connIdx).then((result) => {console.log(JSON.stringify(result));});});}


//openMdsip('spilds.rfx.local:8001').then((connIdx) =>{testIp(connIdx)}, (errorMsg) => {console.log(errorMsg);});
   

var server = http.createServer(function(request, response) {
    if(debug >= 1) console.log((new Date()) + ' Received request for ' + request.url);
    response.writeHead(404);
    response.end();
});
server.listen(webSocketPort, function() {
    if(debug >= 1) console.log((new Date()) + ' Server is listening on port '+webSocketPort);
});

wsServer = new WebSocketServer({
    httpServer: server,
    // You should not use autoAcceptConnections for production
    // applications, as it defeats all standard cross-origin protection
    // facilities built into the protocol and the browser.  You should
    // *always* verify the connection's origin and decide whether or not
    // to accept it.
    autoAcceptConnections: false
});

function originIsAllowed(origin) {
  // put logic here to detect whether the specified origin is allowed.
  return true;
}

function nextRequest(connection, connIdx)
{
    if (pendingReqs[connIdx].length > 0)
    {
        req = pendingReqs[connIdx].shift();
        clientIds[connIdx] = req.clientId;
        requestEvaluate(connection, connIdx, req.experiment, req.shot, req.expression, req.from, req.to);
    }
    else
        busy[connIdx] = false;
}

function requestEvaluate(connection, connIdx, experiment, shot, expression, from, to)
{          
    busy[connIdx] = true;
    var evalExpr = '';
    //if(experiment != experiments[connIdx] || shot != shots[connIdx])
    {
        evalExpr += "treeopen('"+experiment+"',"+shot+"); ";
        experiments[connIdx] = experiment;
        shots[connIdx]= shot;
    }
    sendMdsipReq(evalExpr, connIdx).then((status)=>
    {
        if(debug >= 2) console.log('Treeopen status: '+status[0]);
        if(status[0] & 1)
        {
            evalExpr = '';
            if(from != 0)
                evalExpr += 'SetTimeContext('+from+'QU,'+to+'QU,*); ';
            else
                evalExpr +=  'SetTimeContext('+from+'QU,*,*); ';
            evalExpr += expression;
            if(debug >= 2) console.log(evalExpr);
            sendMdsipReq(evalExpr, connIdx).then((samples)=>
            {
                var exprRes = {};
                exprRes['operation'] = 'evaluate';
                exprRes['connectionId'] = connIdx;
                exprRes['status'] = 'success';
                exprRes['samples'] = samples;
                dimExpression = 'DIM_OF('+expression+')';
                sendMdsipReq(dimExpression, connIdx).then((times)=>
                {
                    exprRes['times'] = times;
                    exprRes['clientId'] = clientIds[connIdx];
                    connection.sendUTF(JSON.stringify(exprRes));
                    nextRequest(connection, connIdx);
                }, (errorMsg) =>
                {
                    var errorRes = {};
                    errorRes['operation'] = 'evaluate';
                    errorRes['status'] = 'failure';
                    errorRes['message'] = errorMsg;
                    connection.sendUTF(JSON.stringify(errorRes));
                    nextRequest(connection, connIdx);
                });
            }, (errorMsg) =>
            {
                var errorRes = {};
                errorRes['operation'] = 'evaluate';
                errorRes['status'] = 'failure';
                errorRes['message'] = errorMsg;
                connection.sendUTF(JSON.stringify(errorRes));
                nextRequest(connIdx);
            });
        }
    }, (errorMsg) =>
    {
        var errorRes = {};
        errorRes['operation'] = 'evaluate';
        errorRes['status'] = 'failure';
        errorRes['message'] = errorMsg;
        connection.sendUTF(JSON.stringify(errorRes));
        nextRequest(connIdx);
    });
}
           


wsServer.on('request', function(request) {
    if (!originIsAllowed(request.origin)) {
      // Make sure we only accept requests from an allowed origin
      request.reject();
      if(debug >= 1) console.log((new Date()) + ' Connection from origin ' + request.origin + ' rejected.');
      return;
    }
    
    var connection = request.accept('mdsip-protocol', request.origin);
    if(debug >= 1) console.log((new Date()) + ' Connection accepted.');
    var client;
    connection.on('message', (function (inConnection)
      { 
        var currConnection = inConnection;
        return function(message)  {
          if (message.type === 'utf8') 
          {
              if(debug >= 2)
              {
                  console.log('Received Message: ' + message.utf8Data);
                  console.log(message);
              }
              var msgObj = JSON.parse(message.utf8Data);
              if(msgObj.operation === 'connect')
              {
                  var ip = msgObj.ip;
                  openMdsip(ip).then((connIdx) =>
                  {
                      var resObj = {};
                      resObj['operation']= 'connect';
                      resObj['status'] = 'success';
                      resObj['connectionId'] = connIdx;
                      currConnection.sendUTF(JSON.stringify(resObj));
                  },
                  (errorMsg) =>
                  {
                      var resObj = {};
                      resObj['operation']= 'connect';
                      resObj['status'] = 'failure';
                      resObj['errorMsg'] = errorMsg;
                      currConnection.sendUTF(JSON.stringify(resObj));
                  });
              }
              else if (msgObj.operation === 'evaluate')
              {
                  var connIdx = msgObj.connectionId;
                  var clientId = msgObj.clientId;
                  var experiment = msgObj.experiment;
                  var shot = msgObj.shot;
                  var expression = msgObj.expression;
                  if(expression == '')
                      return;
                  var from = msgObj.from;
                  var to = msgObj.to;
                  if(busy[connIdx])
                  {
                      var req = {};
                      req['clientId'] = clientId;
                      req['experiment']= experiment;
                      req['shot'] = shot;
                      req['expression'] = expression;
                      req['from'] = from;
                      req['to'] = to;
                      pendingReqs[connIdx].push(req);
                  }
                  else
                  {
                      clientIds[connIdx] = clientId;
                      requestEvaluate(currConnection, connIdx, experiment, shot, expression, from, to);
                  }
              }
              else if(msgObj.operation == 'registerStream')
              {
                  var signal = msgObj.signal;
                  var clientId = msgObj.clientId;
                  var from = msgObj.from;
                  var to = msgObj.to;
                  if(debug >= 1) console.log('Register stream for '+signal);
                  if(signal.trim() == '')
                    return;
                  handleNewConnection(clientId, signal, currConnection, from, to);
              }
              else if(msgObj.operation == 'unregisterStream')
              {
                  var clientId = msgObj.clientId;
                  if(debug >= 1) console.log('Unregister Stream: '+clientId);
                  removeConnectionsFromId(clientId, currConnection)
              }
          } 
        }
    })(connection));
    connection.on('close', (function(inConnection)   
    {
        var currConnection = inConnection;
        return function(reasonCode, description) {
          if(debug >= 1) console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
          removeConnections(currConnection);
        };
    })(connection));

});
    



///////////////////Stream stuff////////////////////
var connections = {};
var history={};
var lastRelativeTimes={};
var lastTimes={};

function handleNewConnection(clientId, name, connection, from, to)
{
    var index = 0;
     //Handle indexing
    if(name.indexOf('[') != -1)
    {   try {
          index = parseInt(name.substring(name.indexOf('[')+1, name.indexOf(']')));
        } catch(err){index = 0;}
        name = name.substring(0,name.indexOf('['));
    }
    if(!(name.toUpperCase in registeredStreams))
      registeredStreams.push(name.toUpperCase());
    if(lastTimes[name] == undefined)
        lastTimes[name] = 0;
    if(connections[name] == undefined)
        connections[name] = [{connection: connection, clientId: clientId, index: index}];
    else
    {
        connections[name].push({connection: connection, clientId: clientId, index: index});
    }
    if(history[name] != undefined)
    {
      //console.log(history[name]);
      var resObj = {};
      resObj['operation'] = 'stream';
      resObj['clientId'] = clientId;
      var retSamples = [];
      var retTimes = [];
      var sampleDim = history[name].samples.length/history[name].times.length;
      if(index >= sampleDim) //Wrong index
        index = 0;
      for(var i = 0; i < history[name].samples.length && history[name].times[i] <= lastTimes[name]; i++)
      {
          if(history[name].times[i] >= from) // && history[name].times[i] <= to)
          {
              retSamples.push(history[name].samples[i*sampleDim + index]);
              retTimes.push(history[name].times[i]);
          }
      }
      resObj['samples']= retSamples;
      resObj['times']= retTimes;
      connection.sendUTF(JSON.stringify(resObj));      
    }
}

function addSamples(name, times, samples)
{
    if(history[name] == undefined )
    {
        history[name] = {times: times, samples: samples};
    }
    else
    {
        for(var i = 0; i < times.length; i++)
        {
            history[name].times.push(times[i]);
        }
        for(var i = 0; i < samples.length; i++)  //There may be a larger number of samples for arrays
        {
            history[name].samples.push(samples[i]);
        }
    }
}

var startAbsTime = 0;
var startRelTime = 0;

function handleEventJSONReception(evMessage)
{
    var configObj = JSON.parse(evMessage);
    console.log(configObj);
    var shot = configObj.shot;
    var name = configObj.name;
    var samples = configObj.samples;
    var times = configObj.times;
    var isAbsoluteTime = configObj.absolute_time == 1;
    if(!isAbsoluteTime)    
    {
        if(lastRelativeTimes[name] == undefined || (lastRelativeTimes[name] > times[0])) 
        {
            startAbsTime =  new Date().getTime();
            startRelTime = times[0]*1000;
        }
        lastRelativeTimes[name] = times[times.length - 1];
    }
    var newTimes = [];
    for(var i = 0; i < times.length; i++)
    {
        if(isAbsoluteTime)
        {
            newTimes.push(times[i]);
        }
        else
        {
            newTimes.push(Math.round(startAbsTime - startRelTime + times[i]*1000));
        }
    }
    addSamples(name, newTimes, samples);
}


function handleEventReception(evMessage)
{
    try {
      var evItems = evMessage.split(' ');
      var shot = parseInt(evItems[0]);
     // if(shot == frozenShot) return;
      var name = evItems[1];
      if(connections[name] == undefined) return; //No connections for this event yet
      var numSamples = parseInt(evItems[3]);
      var times = [];
      var samples = [];
      var isAbsoluteTime = (evItems[2] == 'L');
      if(!isAbsoluteTime)
      {
          if(lastRelativeTimes[name] == undefined || (lastRelativeTimes[name] > parseFloat(evItems[4]))) 
          {
              startAbsTime =  new Date().getTime();
              startRelTime = parseFloat(evItems[4])*1000;
          }
          lastRelativeTimes[name] = parseFloat(evItems[4+numSamples - 1]);

       }
      for(var i = 0; i < numSamples; i++)
      {
          if(isAbsoluteTime)
          {
              times.push(parseInt(evItems[4+i]));
          }
          else
          {
              times.push(Math.round(startAbsTime - startRelTime + parseFloat(evItems[4+i])*1000));
          }
          samples.push(parseFloat(evItems[4+numSamples+i]));
      }
      //if(debug) console.log('Received ' + name, numSamples);
      addSamples(name, times, samples);
    }
    catch(err)
    {
        console.log("Error handling event: " + err.message);
    }
}

function updateConnections()
{
    Object.keys(connections).map(x =>updateConnection(x)); 
}

function updateConnection(name)
{
// console.log('UPDATE CONNECTION '+name+'  '+history[name].times.length);
    lastTime = lastTimes[name];
    var updateTimes = [];
    var updateSamples = [];
    var idx;
    if(history[name] == undefined)
      return;
    for (idx = history[name].times.length - 1; idx > 0 && history[name].times[idx] > lastTime; idx--);
    idx++;
    //console.log(idx + '  '+history[name].times.length);
    if(idx < history[name].times.length)
    {
	lastTimes[name] = history[name].times[history[name].times.length - 1];
        var sampleDim = history[name].samples.length/history[name].times.length; //Shall be > 1 for vector samples
	for(; idx < history[name].times.length; idx++)
	{
	    updateTimes.push(history[name].times[idx]);
            for(var j= 0; j < sampleDim; j++)
                updateSamples.push(history[name].samples[sampleDim*idx+j]);
	}
	
        for(var i = 0; i < connections[name].length; i++)
        {
            var resObj = {};
            resObj['operation'] = 'stream';
            resObj['clientId'] = connections[name][i].clientId;
            var selectedSamples = [];
            var idx = connections[name][i].index;
            if(idx >= sampleDim) //Wrong index
              idx = 0; 
            for(var j = 0; j < updateTimes.length; j++)
              selectedSamples.push(updateSamples[j*sampleDim + idx]);
            resObj['samples']= selectedSamples;
            resObj['times']= updateTimes;
            if(debug >= 2)
            {
                console.log('sending: ')
                console.log(resObj);
            }
              //console.log('Sent');
            connections[name][i].connection.sendUTF(JSON.stringify(resObj)); 
        }
    }
}
	
setInterval(updateConnections, refreshRate);





function removeConnections(connection)
{
    Object.keys(connections).map(x =>removeConnection(x, connection)); 
}


function removeConnection(name, connection)
{
    if(connections[name] == undefined)
        return;
    var i;
    for(i = 0; i < connections[name].length; i++)
    {
        if(connections[name][i].connection == connection)
            break;
    }
    if(i < connections.length)
        connections[name].splice(i, 1);
}


function removeConnectionsFromId(clientId, connection)
{
    Object.keys(connections).map(x =>removeConnectionFromId(x, clientId, connection)); 
}


function removeConnectionFromId(name, clientId, connection)
{
   if(debug >= 1) console.log('remove '+name+' '+clientId +'  ' +connection);
  //console.log(connections[name]);
    if(connections[name] == undefined)
        return;
    var i;
    for(i = 0; i < connections[name].length; i++)
    {
        if(connections[name][i].clientId == clientId && connections[name][i].connection == connection)
            break;
    }
    if(i < connections[name].length)
    {
        connections[name].splice(i, 1);
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
  
//  console.log('Event from ' + rinfo.address+':'+rinfo.port+'   '+len + '   '+ eventName + '  '+dataLen);
//  if(dataLen > 0)
//      console.log(eventData);
  
  if(eventName == 'STREAMING'  || registeredStreams.indexOf(eventName.toUpperCase()) >= 0)
  {
      if(eventData[0] == '{')
        handleEventJSONReception(eventData)
      else
        handleEventReception(eventData);
  }
  else if(eventName == 'STREAMING_FREEZE')
  {
     //handleFreezeShot(eventData);
  }
  else if (eventName = 'STREAMING_UNFREEZE')
  {
     // handleUnfreezeShot()
  }
}

var eventPort;
  try {
     eventPort= parseInt(process.env['mdsevent_port']);
     if (process.env['mdsevent_port'] == undefined)
        eventPort = 4000
  }catch(err) {eventPort = 4000;}

eventServer.bind(eventPort, function(){
    eventServer.setBroadcast(true);
    eventServer.setMulticastTTL(128);
    var multicastAddress;
    try {
        multicastAddress = process.env['mdsevent_address'];
        if (multicastAddress != undefined)
        {
          eventServer.addMembership(multicastAddress);
          if(debug >= 1) console.log('Multicast Address: ' + multicastAddress);
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


