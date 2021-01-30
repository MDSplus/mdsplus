

//var mdsipNet = require('net');
var clients = {};
var connectResolve;
var connectReject;
var resolves = {};
var rejects = {};
var W3CWebSocket = require('websocket').w3cwebsocket;
//var WebSocketClient = require('websocket').client;
var webSocketConnection;

var client;
var msgId = 0;

export var openMdsip = function(ip, port)
{
//   client = new W3CWebSocket('ws://spilds.rfx.local:8020/', 'mdsip-protocol');
   client = new W3CWebSocket('ws://'+location.hostname+':'+port+'/', 'mdsip-protocol');

   client.onerror=function(error) {
        connectReject('WebSocketServer not responding:'+error.toString());
        console.log('Connect Error: ' + error.toString());
    };

    client.onopen=function(connection) {
        webSocketConnection = connection;
        var reqObj = {};
        reqObj['operation'] = 'connect';
        reqObj['ip'] = ip;
        client.send(JSON.stringify(reqObj));
        connection.onerror = function(error) {
            connectReject(error.toString());
            console.log("Websocket error: " + error.toString());
        };
        connection.onclose = function() {
            console.log('echo-protocol Connection Closed');
            connectReject('WebSocketServer closed');
        }
    };
        
    client.onmessage  = function(message) 
    {
        if (typeof message.data === 'string') 
        {
            var resObj = JSON.parse(message.data);
            if(resObj.operation == 'connect')                
            {
                if(resObj.status == 'success')
                {
                    connectResolve(resObj.connectionId);
                }
                else
                {
                    connectReject(resObj.errorMsg);
                }
            }
            else if(resObj.operation == 'evaluate')
            {
                //var connIdx = resObj.connectionId;
                var clientId = resObj.clientId;
                if(resObj.status == 'success')
                {
                     resolves[clientId]([resObj.samples, resObj.times]);
                }
                else
                {
                     rejects[clientId](resObj.errorMsg);
                }
             }
        }
    };
    return new Promise((resolve, reject)=>{ connectResolve = resolve; connectReject = reject});
 }  
    

export var sendMdsipReq = function(experiment, shot, expression, from, to, connIdx)
 {
	if(client == undefined)
	{
		console.log('server  not connected');
                if(rejects[connIdx] != undefined)
                    rejects[connIdx]('Not Connected');
		return;
	}
    var reqObj = {};
    reqObj['operation'] = 'evaluate';
    reqObj['experiment'] = experiment;
    reqObj['shot']= shot;
    reqObj['expression'] = expression;
    reqObj['from'] = from;
    reqObj['to'] = to;
    reqObj['connectionId'] = connIdx;
    reqObj['clientId'] = msgId;
    msgId++;
    client.send(JSON.stringify(reqObj));
	return new Promise((resolve, reject) => {
        resolves[msgId - 1] = resolve;
        rejects[msgId - 1] =  reject;
    });
 }

