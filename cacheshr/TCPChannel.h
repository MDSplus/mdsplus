#ifndef TCP_CHANNEL_H
#define TCP_CHANNEL_H
#include "CommunicationChannel.h"
#include "IPAddress.h"


class TCPServer:public Runnable
{
	CommunicationChannel *channel;
	int sock;
	Thread thread;

public:
	TCPServer(CommunicationChannel *channel, int sock)
	{
		this->channel = channel;
		this->sock = sock;
	}
	void run(void *args);
};

class TCPHandler:public Runnable
{
	CommunicationChannel *channel;
	int sock;

public:
	TCPHandler(CommunicationChannel *channel, int sock)
	{
		this->channel = channel;
		this->sock = sock;
	}
	void run(void *args);
};



class TCPChannel:public CommunicationChannel
{
	int tcpSocket;
	Thread thread;


public:
	TCPChannel(int idx);
	bool connectSender(ChannelAddress *addr);
	bool connectReceiver(ChannelAddress *addr);
	bool sendMessage(ChannelAddress *addr, char *buf, int bufLen, char type);
	unsigned int fromNative(int n);
	int toNative(unsigned int l);
};


#endif
