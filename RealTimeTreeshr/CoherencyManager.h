#ifndef COHERENCY_MANAGER_H
#define COHERENCY_MANAGER_H
#include "SharedDataManager.h"
#include "CommunicationChannel.h"
#include "ChannelAddress.h"
#include "ChannelFactory.h"
#include "Runnable.h"


//Message types
#define REQUEST_DATA_TYPE 1
#define OWNERSHIP_TYPE 2
#define OWNERSHIP_ACK_TYPE 3
#define DATA_TYPE 4
#define DATA_ACK_TYPE 5



class CoherencyManager:ChannelListener
{
	SharedDataManager *dataManager;
	CommunicationChannel *channel;
	ChannelFactory chanFactory;

	void handleRequestDataMsg(int nid, ChannelAddress *addr);
	void handleOwnershipMsg(int nid, int timestamp, char ownerIdx, ChannelAddress *addr);
	void getData(int nid, int numSlots, bool append, char *buf);
	void handleDataMsg(int nid, bool append, bool ackRequest, char *buf, ChannelAddress *addr);
	char *getFullDataBuffer(int nid, int &dataLen);
	char *getUpdateDataMsg(int nid, int &msgLen);


public:
	void handleMessage(ChannelAddress *addr, char *buf, int bufLen, char type);
	CoherencyManager(SharedDataManager *dataManager);
	void checkRead(int nid);
	void checkWrite(int nid);
	void startMessageHandler();
	void startServer();
};

#endif