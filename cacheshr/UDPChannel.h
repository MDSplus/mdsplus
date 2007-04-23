#ifndef UDP_CHANNEL_H
#define UDP_CHANNEL_H
#include "CommunicationChannel.h"
#include "IPAddress.h"

#define MAX_MSG_LEN 64000

class UDPServer:public Runnable
{
	CommunicationChannel *channel;
	int socket;
	struct sockaddr_in retAddress;
	Thread thread;

public:
	UDPServer(CommunicationChannel *channel, int socket)
	{
		this->channel = channel;
		this->socket = socket;
	}
	void run(void *args);
};




class UDPChannel:public CommunicationChannel
{
	int udpSocket;
	Thread thread;


public:
	UDPChannel(int idx);
	bool connectSender(ChannelAddress *addr);
	bool connectReceiver(ChannelAddress *addr);
	bool sendMessage(ChannelAddress *addr, char *buf, int bufLen, char type);
	unsigned int fromNative(int n);
	int toNative(unsigned int l);
};


#endif
