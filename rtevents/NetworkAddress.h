#ifndef NETWORKADDRESS_H_
#define NETWORKADDRESS_H_
#include "SystemSpecific.h"

class EXPORT NetworkAddress
{
public:
	virtual char *getAddressString() = 0;
	virtual void setAddressString(char *addrStr) = 0;
	virtual bool equals(NetworkAddress *addr) = 0;
	virtual void print() = 0;
};


#endif /*NETWORKADDRESS_H_*/
