#ifndef TCPMESSAGEMANAGER_H_
#define TCPMESSAGEMANAGER_H_
#include "MessageManager.h"
#include "IPAddress.h"
#include "Runnable.h"
#include "Thread.h"

class TCPServer:public Runnable
{
	int sock;
	MessageManager *manager;
	Thread thread;

public:
	TCPServer(MessageManager *manager, int sock)
	{
		this->sock = sock;
		this->manager = manager;
	}
	void run(void *args);
};

class TCPHandler:public Runnable
{
	IPAddress *addr;
	MessageManager *manager;
public:
	TCPHandler(MessageManager *manager, IPAddress *addr)
	{
		this->manager = manager;
		this->addr = addr;
	}
	void run(void *args);
};



class TCPMessageManager:public MessageManager
{
	friend class TCPMessageSender;
	
	bool TCPMessageManager::connectSender(NetworkAddress *addr);

public:
	virtual bool TCPMessageManager::sendMessage(NetworkAddress *addr, char *buf, int bufLen);
	virtual MessageSender *getSender(NetworkAddress *addr);
	virtual bool connectReceiver(NetworkAddress *addr, MessageReceiver *receiver);
	virtual unsigned int fromNative(unsigned int n);
	virtual unsigned int toNative(unsigned int n);
};

#endif /*TCPMESSAGEMANAGER_H_*/
