#ifndef TCPNETWORKMANAGER_H_
#define TCPNETWORKMANAGER_H_
#include "SystemSpecific.h"
#include "IPAddress.h"
#include "Runnable.h"
#include "Thread.h"
#include "NetworkManager.h"

class TCPServer:public Runnable
{
	int sock;
	int port;
	NetworkManager *manager;
	Thread thread;

public:
	TCPServer(NetworkManager *manager, int sock, int port)
	{
		this->sock = sock;
		this->port = port;
		this->manager = manager;
	}
	virtual void run(void *args);
};

class TCPHandler:public Runnable
{
	IPAddress *addr;
	NetworkManager *manager;

public:
	TCPHandler(NetworkManager *manager, IPAddress *addr)
	{
		this->manager = manager;
		this->addr = new IPAddress(addr);
	}
	void run(void *args);
};



class EXPORT TCPNetworkManager:public NetworkManager
{
	
	bool connectSender(NetworkAddress *addr);

public:
	virtual bool sendMessage(NetworkAddress *addr, char *buf, int bufLen);
	virtual bool connectReceiver(NetworkAddress *addr, NetworkReceiver *receiver);
	virtual unsigned int fromNative(unsigned int n);
	virtual unsigned int toNative(unsigned int n);
};

#endif /*TCPNETWORKMANAGER_H_*/
