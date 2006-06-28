#include <stdio.h>
#include "CommunicationChannel.h"

CommunicationChannel::CommunicationChannel()
{
	for(int i = 0; i < MAX_TYPES; i++)
	{
		types[i] = -1;
		numListeners[i] = 0;
	}
	numTypes = 0;
}


void CommunicationChannel::messageReceived(ChannelAddress *addr, char type, char *buf, int bufLen)
{
	int typeIdx;


	lock.lock();

	for(typeIdx = 0; typeIdx < numTypes && type != types[typeIdx]; typeIdx++);
	if(typeIdx < numTypes) //It is an expected type
	{
		for (int j = 0; j < numListeners[typeIdx]; j++)
		{
			listeners[typeIdx][j]->handleMessage(addr, buf, bufLen, type);
		}
	}

	lock.unlock();
}

void CommunicationChannel::attachListener(ChannelListener *listener, char type)
{
	int i;
	for(i = 0; i < MAX_TYPES && types[i] != -1 && types[i] != type; i++);
	if(i >= MAX_TYPES)
	{
		printf("Internal error in Reliable channel: no more types available\n");
		return;
	}

	if(types[i] == -1)
	{
		types[i] = type;
		numListeners[i] = 1;
		listeners[i][0] = listener;
		numTypes++;
	}
	else
		listeners[i][numListeners[i]++] = listener;
}

