#include "UDPChannel.h"
bool UDPChannel::initialized;


void UDPServer::run(void *arg)
{
//TCP Message protocol: type (1 byte), length (4 bytes), message (length bytes) 

	IPAddress addr;
	char senderIdx;
	int recBytes;
	char type;
	static	char recBuf[MAX_MSG_LEN];
    struct sockaddr_in clientAddr;
	addr.socket = socket;
	int addrSize = sizeof(clientAddr);

	while(true)
	{
		if((recBytes = recvfrom(socket, (char *)recBuf, MAX_MSG_LEN, 0, 
			(struct sockaddr *)&clientAddr, &addrSize)) < 0)
    	{
			printf("Error receiving UDP messages\n");
        }
		type = recBuf[0];
		senderIdx = recBuf[1];
	
		channel->messageReceived(&addr, (int)senderIdx, type, &recBuf[2], recBytes - 2);
	}
}




UDPChannel::UDPChannel(int idx):CommunicationChannel(idx)
{
#ifdef HAVE_WINDOWS_H

    WSADATA wsaData;
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(1,1);

	if(!initialized)
	{
		initialized = true;
		WSAStartup(wVersionRequested,&wsaData);
	}

#endif
}




bool UDPChannel::connectSender(ChannelAddress *addr)
{
	IPAddress *ipAddr = (IPAddress *)addr;
	ipAddr->socket= socket(AF_INET, SOCK_DGRAM, 0);
#ifdef HAVE_WINDOWS_H
	if (ipAddr->socket == INVALID_SOCKET) return false;
#else
	if (ipAddr->socket == -1) return false;
#endif
	return true;
}



bool UDPChannel::connectReceiver(ChannelAddress *address)
{
	char *ipAddress = ((IPAddress *)address)->ipAddress;
	int port = ((IPAddress *)address)->port;

	int addrSize = sizeof(struct sockaddr_in), retAddrSize = 0;

	if((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		printf("Error creating socket\n");
		return false;
	}

#ifdef HAVE_WINDOWS_H
	if(bind(udpSocket, (SOCKADDR *)&((IPAddress *)address)->sin, sizeof(((IPAddress *)address)->sin)) != 0)
#else
	if(bind(udpSocket, (struct sockaddr *)&((IPAddress *)address)->sin, sizeof((IPAddress *)address)->sin)) != 0)
#endif
	{   
		printf("Cannot bind socket at port %d\n", port);
		return false;
	}


	UDPServer *udpServer = new UDPServer(this, udpSocket);
	thread.start(udpServer, 0);
	return true;
}


bool UDPChannel::sendMessage(ChannelAddress *addr, char *buf, int bufLen, char type)
{
	int socket = ((IPAddress *)addr)->socket;

	if(socket == -1) //Not connected yet
	{
		if(!connectSender(addr))
			return false; //Still unsuccesful
		socket = ((IPAddress *)addr)->socket;
	}
	char *outBuf = new char[2+bufLen];
	outBuf[0] = type;
	outBuf[1] = thisIdx;
	memcpy(&outBuf[2], buf, bufLen);
    if(sendto(socket, outBuf, (bufLen+2 < MAX_MSG_LEN)?(bufLen + 2):MAX_MSG_LEN, 0, 
		(struct sockaddr *)&((IPAddress *)addr)->sin, sizeof(((IPAddress *)addr)->sin))==-1)
    {
		printf("Error sending UDP message!\n");
		return false;
    }
	delete [] outBuf;
    return true;
} 




unsigned int UDPChannel::fromNative(int n)
{
	return htonl(n);
}

int UDPChannel::toNative(unsigned int n)
{
	return ntohl(n);
}


