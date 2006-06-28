#ifndef CHANNEL_ADDRESS_H
#define CHANNEL_ADDRESS_H

class ChannelAddress
{
public:
	virtual char *getAddressString() = 0;
	virtual void setAddressString(char *addrStr) = 0;
};

#endif