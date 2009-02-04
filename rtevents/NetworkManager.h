#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include "SystemSpecific.h"
#include "NetworkAddress.h"
#include "NetworkReceiver.h"

//Class Message Manager is the common interface for exchanging messages
//over a given network 
class EXPORT NetworkManager
{
public:
	virtual bool connectReceiver(NetworkAddress *addr, NetworkReceiver *receiver) = 0;
	virtual unsigned int fromNative(unsigned int n) = 0;
	virtual unsigned int toNative(unsigned int n) = 0;
	virtual bool sendMessage(NetworkAddress *addr, char *buf, int bufLen) = 0;
};
#endif /*MESSAGEMANAGER_H_*/
