#ifndef MESSAGERECEIVER_H_
#define MESSAGERECEIVER_H_
#include "SystemSpecific.h"
#include "NetworkAddress.h"
class EXPORT MessageReceiver 
{
public:
	virtual void messageReceived(NetworkAddress *addr, char *buf, int size) = 0;
	virtual void connectionTerminated(NetworkAddress *addr) = 0;
};
#endif /*MESSAGERECEIVER_H_*/
