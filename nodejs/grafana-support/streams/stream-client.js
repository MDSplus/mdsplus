

//var mdsipNet = require('net');
var clients = {};
var connectResolve;
var connectReject;
var resolves = {};
var W3CWebSocket = require('websocket').w3cwebsocket;
//var WebSocketClient = require('websocket').client;
var webSocketConnection;

var client;
var msgId = 0;
var portNumber = 8020;

export var connectToStreams = function(inPortNumber)
{
  portNumber = inPortNumber;
//   client = new W3CWebSocket('ws://spilds.rfx.local:8020/', 'mdsip-protocol');
   client = new W3CWebSocket('ws://'+location.hostname+':'+portNumber+'/', 'mdsip-protocol');

   client.onerror=function(error) {
        connectReject('WebSocketServer not responding:'+error.toString());
        console.log('Connect Error: ' + error.toString());
        client = undefined;
    };

    client.onopen=function(connection) {
        webSocketConnection = connection;
        connectResolve('Websocket connected');
        connection.onerror = function(error) {
            console.log("Websocket error: " + error.toString());
            connectReject(error.toString());
        };
        connection.onclose = function() {
            console.log('echo-protocol Connection Closed');
            connectReject('WebSocketServer closed');
        };
        connection.onerror = function() {
          client = undefined;
        };
    };
        
    client.onmessage  = function(message) 
    {
        if (typeof message.data === 'string') 
        {
            var resObj = JSON.parse(message.data);
            if(resObj.operation == 'stream')
            {
                var clientId = resObj.clientId;
                resolves[clientId](resObj.samples,resObj.times);
            }
        }
    };
    return new Promise((resolve, reject)=>{ connectResolve = resolve; connectReject = reject});
 }  
    
export var unregisterStream = function(clientId)
{
    if(client == undefined)
      return;
    var reqObj = {};
    reqObj['operation'] = 'unregisterStream';
    reqObj['clientId'] = clientId;
    msgId++;
    client.send(JSON.stringify(reqObj));
 }
export var registerStream = function(signal, from, to, update)
 {
    if(client == undefined)
    {    
       connectToStreams(portNumber).then((resolveMsg) => {
         if(client != undefined)
         {
            var reqObj = {};
            reqObj['operation'] = 'registerStream';
            reqObj['signal'] = signal;
            reqObj['clientId'] = msgId;
            reqObj['from'] = from;
            reqObj['to'] = to;
            client.send(JSON.stringify(reqObj));
            resolves[msgId] = update;
         }
       });
       msgId++;
       return msgId - 1;
    }
    else
    {
        var reqObj = {};
        reqObj['operation'] = 'registerStream';
        reqObj['signal'] = signal;
        reqObj['clientId'] = msgId;
        reqObj['from'] = from;
        msgId++;
        client.send(JSON.stringify(reqObj));
        resolves[msgId - 1] = update;
        return msgId - 1;
    }
 }

