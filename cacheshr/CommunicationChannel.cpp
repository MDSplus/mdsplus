#include <stdio.h>
#include "CommunicationChannel.h"




CommunicationChannel::CommunicationChannel(int idx)
{
	thisIdx = idx;
	for(int i = 0; i < MAX_TYPES; i++)
	{
		types[i] = -1;
		numListeners[i] = 0;
	}
	numTypes = 0;
}

/*void CommunicationChannel::run(void *arg)
{
	HandlerArgs *handlerArgs = (HandlerArgs *)arg;
	handlerArgs->chanListener->handleMessage(handlerArgs->addr, handlerArgs->senderIdx, handlerArgs->buf, handlerArgs->bufLen,
		handlerArgs->type);
	delete handlerArgs;
}

*/

void CommunicationChannel::messageReceived(ChannelAddress *addr, int senderIdx, char type, char *buf, int bufLen)
{
	int typeIdx;


	lock.lock();

	for(typeIdx = 0; typeIdx < numTypes && type != types[typeIdx]; typeIdx++);
	if(typeIdx < numTypes) //It is an expected type
	{
		for (int j = 0; j < numListeners[typeIdx]; j++)
		{
/*			HandlerArgs *args = new HandlerArgs;
			args->chanListener = listeners[typeIdx][j];
			args->senderIdx = senderIdx;
			args->addr = addr;
			args->buf = buf;
			args->bufLen = bufLen;
			args->type = type;
			thread.start(this, args);
*/
			listeners[typeIdx][j]->handleMessage(addr, senderIdx, buf, bufLen, type);
		}
	}

	lock.unlock();
}

void CommunicationChannel::attachListener(ChannelListener *listener, char type)
{
	int i;
	for(i = 0; i < MAX_TYPES && types[i] != -1 && types[i] != type; i++)
		printf("%d\t%d\n", types[i], type);
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

