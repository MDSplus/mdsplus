#ifndef TCP_CHANNEL_H
#define TCP_CHANNEL_H
#include "CommunicationChannel.h"
#include "TCPAddress.h"




class TCPChannel:public CommunicationChannel
{
	int tcpSocket;
	static bool initialized;



public:
	TCPChannel();
	bool connectSender(ChannelAddress *addr);
	bool connectReceiver(ChannelAddress *addr);
	bool sendMessage(ChannelAddress *addr, char *buf, int bufLen, char type);
	char *receiveMessage(ChannelAddress *addr, int *retLen, char *retType);
	unsigned int fromNative(int n);
	int toNative(unsigned int l);
};


#endif