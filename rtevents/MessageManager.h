#ifndef MESSAGEMANAGER_H_
#define MESSAGEMANAGER_H_

#include "SystemSpecific.h"
#include "NetworkAddress.h"
#include "MessageReceiver.h"
#include "MessageSender.h"

//Class Message Manager is the common interface for exchanging messages
//over a given network 
class EXPORT MessageManager
{
public:
	virtual MessageSender *getSender(NetworkAddress *addr) = 0;
	virtual bool connectReceiver(NetworkAddress *addr, MessageReceiver *receiver) = 0;
	virtual unsigned int fromNative(unsigned int n) = 0;
	virtual unsigned int toNative(unsigned int n) = 0;
	virtual bool sendMessage(NetworkAddress *addr, char *buf, int bufLen) = 0;
};
#endif /*MESSAGEMANAGER_H_*/
