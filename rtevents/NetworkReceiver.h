#ifndef NETWORKRECEIVER_H_
#define NETWORKRECEIVER_H_
#include "SystemSpecific.h"
#include "NetworkAddress.h"
class EXPORT NetworkReceiver 
{
public:
	virtual void messageReceived(NetworkAddress *addr, char *buf, int size) = 0;
	virtual void connectionTerminated(NetworkAddress *addr) = 0;
};
#endif /*MESSAGERECEIVER_H_*/
