#ifndef CHANNEL_LISTENER_H
#define CHANNEL_LISTENER_H
#include "ChannelAddress.h"

class ChannelListener
{
public:
	virtual void handleMessage(ChannelAddress *addr, char *buf, int bufLen, char type) = 0;
};

#endif