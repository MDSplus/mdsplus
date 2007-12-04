#ifndef COHERENCY_MANAGER_H
#define COHERENCY_MANAGER_H
#include "SharedDataManager.h"
#include "CommunicationChannel.h"
#include "ChannelAddress.h"
#include "ChannelFactory.h"
#include "Runnable.h"
#include "Thread.h"


//Message types
#define REQUEST_DATA_TYPE 1
#define OWNERSHIP_TYPE 2
#define OWNERSHIP_WARM_ACK_TYPE 3
#define DATA_TYPE 4
#define DIRTY_TYPE 6

#define WAS_OWNER 1
#define WARM 2
#define NO_WARM 3
#define INVALID 4



class CoherencyManager:ChannelListener
{
	SharedDataManager *dataManager;
	CommunicationChannel *channel;
	ChannelFactory chanFactory;

	void handleRequestDataMsg(int treeIdx, int nid, ChannelAddress *senderAddr, int senderIdx);
	void handleOwnershipMsg(int treeIdx, int nid, int timestamp, char ownerIdx, ChannelAddress *senderAddr, int senderIdx);
	void handleDirtyMsg(int treeIdx, int nid, ChannelAddress *senderAddr, int senderIdx);
	void handleDataMsg(int treeIdx, int nid, char *buf, int bufLen, ChannelAddress *senderAddr, int senderIdx);
	void handleOwnershipWarmMessage(int treeIdx, int nid, ChannelAddress *senderAddr, int senderIdx);

 
public:
	void handleMessage(ChannelAddress *addr, int senderIdx, char *buf, int bufLen, char type);
	CoherencyManager(SharedDataManager *dataManager);
	void checkRead(int treeIdx, int nid);
	void checkWrite(int treeIdx, int nid);
	void startMessageHandler();
	void startServer();
};

#endif
