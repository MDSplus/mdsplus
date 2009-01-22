#ifndef TCPMESSAGEMANAGER_H_
#define TCPMESSAGEMANAGER_H_
#include "SystemSpecific.h"
#include "IPAddress.h"
#include "Runnable.h"
#include "Thread.h"
#include "MessageManager.h"

class TCPServer:public Runnable
{
	int sock;
	int port;
	MessageManager *manager;
	Thread thread;

public:
	TCPServer(MessageManager *manager, int sock, int port)
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
	MessageManager *manager;

public:
	TCPHandler(MessageManager *manager, IPAddress *addr)
	{
		this->manager = manager;
		this->addr = new IPAddress(addr);
	}
	void run(void *args);
};



class EXPORT TCPMessageManager:public MessageManager
{
	friend class TCPMessageSender;
	
	bool connectSender(NetworkAddress *addr);

public:
	virtual bool sendMessage(NetworkAddress *addr, char *buf, int bufLen);
	virtual MessageSender *getSender(NetworkAddress *addr);
	virtual bool connectReceiver(NetworkAddress *addr, MessageReceiver *receiver);
	virtual unsigned int fromNative(unsigned int n);
	virtual unsigned int toNative(unsigned int n);
};

#endif /*TCPMESSAGEMANAGER_H_*/
