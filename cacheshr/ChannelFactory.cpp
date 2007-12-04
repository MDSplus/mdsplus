#include "ChannelFactory.h"
#include <stdio.h>

#define TCP_PORT 4000

static 	ChannelAddress *addresses[512];
static 	ChannelAddress *otherAddresses[512];
static  ChannelAddress *thisAddress;
static int numChanAddresses;
static int thisAddressIdx;

//Configuration file format: This IP address, followed by the whole list of IP addresses

ChannelFactory::ChannelFactory()
{
//TEMPORANEO
	communicationEnabled = false;
	return;



	if(numChanAddresses == 0)
	{
		int addrIdx = 0;
		int otherAddrIdx = 0;
		ChannelAddress *currAddr;

		FILE *confFile = fopen("realtime_mdsip.hosts", "r");
		if(!confFile)
		{
			//printf("Cannot open configuration file\n");
			communicationEnabled = false;
			return;
		}
		char line[512];
		char thisIp[512];
		fgets(line, 512, confFile);
		sscanf(line, "%s", thisIp);
		int idx = 0;
		while(fgets(line, 512, confFile))
		{
			char currIp[512];
			sscanf(line, "%s", currIp);
			currAddr = addresses[addrIdx++] = new IPAddress(currIp, TCP_PORT);
			if(strcmp(thisIp, currIp))
			{
				otherAddresses[otherAddrIdx++] = currAddr;
			}
			else
			{
				thisIdx = idx;
				thisAddress = currAddr;
				thisAddressIdx = addrIdx - 1;
			}
			idx++;
		}
		numChanAddresses = addrIdx;
		communicationEnabled = true;
	}
}


int ChannelFactory::getThisAddressIdx()
{
	return thisAddressIdx;
}

ChannelAddress *ChannelFactory::getAddress(int idx)
{
	return addresses[idx];
}


ChannelAddress *ChannelFactory::getThisAddress()
{
	return thisAddress;
}


ChannelAddress **ChannelFactory::getOtherAddresses(int &numAddresses)
{
	numAddresses = numChanAddresses - 1;
	return otherAddresses;
}

CommunicationChannel *ChannelFactory::getChannel()
{
	if(!communicationEnabled) return 0;
//	return new TCPChannel(thisIdx);
	return new UDPChannel(thisIdx);
}

