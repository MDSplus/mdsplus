#ifndef UDPNETWORKMANAGER_H
#define UDPNETWORKMANAGER_H
#include "NetworkManager.h"
#include "IPAddress.h"
#include "Thread.h"
#include "SystemSpecific.h"

#define MAX_MSG_LEN 64000

class UDPServer:public Runnable
{
	NetworkManager *channel;
	int sock;
	struct sockaddr_in retAddress;
	Thread thread;

public:
	UDPServer(NetworkManager *channel, int sock)
	{
		this->channel = channel;
		this->sock = sock;
	}
	void run(void *args);
};



class EXPORT UDPNetworkManager:public NetworkManager
{
	int udpSocket;

	bool connectSender(NetworkAddress *addr);

public:
	virtual bool sendMessage(NetworkAddress *addr, char *buf, int bufLen);
	virtual bool connectReceiver(NetworkAddress *addr, NetworkReceiver *receiver);
	virtual unsigned int fromNative(unsigned int n);
	virtual unsigned int toNative(unsigned int n);
};



#endif
