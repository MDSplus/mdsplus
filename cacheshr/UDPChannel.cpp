#include "UDPChannel.h"
bool IPAddress::initialized;


void UDPServer::run(void *arg)
{
//TCP Message protocol: type (1 byte), length (4 bytes), message (length bytes) 

	IPAddress addr;
	char senderIdx;
	int recBytes;
	char type;
	static	char recBuf[MAX_MSG_LEN];
    struct sockaddr_in clientAddr;
	addr.sock = sock;
	int addrSize = sizeof(clientAddr);

	while(true)
	{

#ifdef HAVE_WINDOWS_H
		if((recBytes = recvfrom(sock, (char *)recBuf, MAX_MSG_LEN, 0, 
			(struct sockaddr *)&clientAddr, &addrSize)) < 0)
#else
#ifdef HAVE_VXWORKS_H
		if((recBytes = recvfrom(sock, (char *)recBuf, MAX_MSG_LEN, 0, 
			(struct sockaddr *)&clientAddr, &addrSize)) < 0)
#else
		if((recBytes = recvfrom(sock, (char *)recBuf, MAX_MSG_LEN, 0, 
			(struct sockaddr *)&clientAddr, (socklen_t *)&addrSize)) < 0)
#endif
#endif
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
}




bool UDPChannel::connectSender(ChannelAddress *addr)
{
	IPAddress *ipAddr = (IPAddress *)addr;
	ipAddr->sock= socket(AF_INET, SOCK_DGRAM, 0);
#ifdef HAVE_WINDOWS_H
	if (ipAddr->sock == INVALID_SOCKET) return false;
#else
	if (ipAddr->sock == -1) return false;
#endif
	return true;
}



bool UDPChannel::connectReceiver(ChannelAddress *address)
{
    struct sockaddr_in serverAddr;
//    serverAddr.sin_len = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_len = (u_char)sizeof(struct sockaddr_in);
    serverAddr.sin_port = htons(((IPAddress *)address)->port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	char *ipAddress = ((IPAddress *)address)->ipAddress;
	int port = ((IPAddress *)address)->port;

	int addrSize = sizeof(struct sockaddr_in), retAddrSize = 0;

	if((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		printf("Error creating socket\n");
		return false;
	}

#ifdef HAVE_WINDOWS_H
	if(bind(udpSocket, (SOCKADDR *)&serverAddr, sizeof(((IPAddress *)address)->sin)) != 0)
#else
	if(bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(((IPAddress *)address)->sin)) != 0)
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
	int sock = ((IPAddress *)addr)->sock;
	if(sock == -1) //Not connected yet
	{
		if(!connectSender(addr))
			return false; //Still unsuccesful
		sock = ((IPAddress *)addr)->sock;
	}
	char *outBuf = new char[2+bufLen];
	outBuf[0] = type;
	outBuf[1] = thisIdx;
	memcpy(&outBuf[2], buf, bufLen);

    if(sendto(sock, outBuf, (bufLen+2 < MAX_MSG_LEN)?(bufLen + 2):MAX_MSG_LEN, 0, 
		(struct sockaddr *)&((IPAddress *)addr)->sin, sizeof(((IPAddress *)addr)->sin))==-1)
    {
		printf("Error sending UDP message!\n");
#ifdef HAVE_WINDOWS_H
		int error = WSAGetLastError();
		switch(error)
		{
			case WSANOTINITIALISED: printf("WSAENETDOWN\n"); break;
			case WSAENETDOWN: printf("WSAENETDOWN\n"); break; 
			case WSAEADDRINUSE: printf("WSAEADDRINUSE\n"); break;
			case WSAEINTR : printf("WSAEINTR\n"); break;
			case WSAEINPROGRESS: printf("WSAEINPROGRESS\n"); break;
			case WSAEALREADY: printf("WSAEALREADY\n"); break;
			case WSAEADDRNOTAVAIL: printf("WSAEADDRNOTAVAIL\n"); break;
			case WSAEAFNOSUPPORT: printf("WSAEAFNOSUPPORT\n"); break;
			case WSAECONNREFUSED : printf("WSAECONNREFUSED\n"); break;
			case WSAEFAULT : printf("WSAEFAULT\n"); break;
			default: printf("BOH\n");
		}
#endif
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


