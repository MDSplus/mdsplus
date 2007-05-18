#ifndef UDP_CHANNEL_H
#define UDP_CHANNEL_H
#include "CommunicationChannel.h"
#include "IPAddress.h"

#define MAX_MSG_LEN 64000

class UDPServer:public Runnable
{
	CommunicationChannel *channel;
	int sock;
	struct sockaddr_in retAddress;
	Thread thread;

public:
	UDPServer(CommunicationChannel *channel, int sock)
	{
		this->channel = channel;
		this->sock = sock;
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
