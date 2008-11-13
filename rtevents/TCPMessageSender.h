#ifndef TCPMESSAGESENDER_H_
#define TCPMESSAGESENDER_H_
#include "TCPMessageManager.h"

class TCPMessageSender: public MessageSender
{
	TCPMessageManager *manager;
	IPAddress *address;
public:
	TCPMessageSender(IPAddress *address, TCPMessageManager *manager)
	{
		this->manager = manager;
		this->address = address;
	}
	virtual void sendMessage(char *buf, int size)
	{
		manager->sendMessage(address, buf, size);
	}
	NetworkAddress *getAddress()
	{
		return address;
	}
};

#endif /*TCPMESSAGESENDER_H_*/
