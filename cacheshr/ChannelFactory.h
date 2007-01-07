#ifndef CHANNEL_FACTORY_H
#define CHANNEL_FACTORY_H

#include "CommunicationChannel.h"
#include "TCPChannel.h"
#include "ChannelAddress.h"
#include "TCPAddress.h"

class ChannelFactory
{
	int thisIdx; //Idx (index in realtime_mdsplus address list) of this node
	bool communicationEnabled;
public:
	ChannelFactory();

	int getThisAddressIdx();
	ChannelAddress *getAddress(int idx);
	ChannelAddress *getThisAddress();
	ChannelAddress **getOtherAddresses(int &numAddresses);	
	
	CommunicationChannel *getChannel();
	bool isCommunicationEnabled(){return communicationEnabled;}
};



#endif
