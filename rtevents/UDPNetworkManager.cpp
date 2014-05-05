#include "UDPNetworkManager.h"

void UDPServer::run(void *arg)
{
//TCP Message protocol: type (1 byte), length (4 bytes), message (length bytes) 

	IPAddress *addr;
	int recBytes;
	static	char recBuf[MAX_MSG_LEN];
    struct sockaddr_in clientAddr;
	//addr.sock = sock;
	int addrSize = sizeof(clientAddr);

	NetworkReceiver *msgReceiver = (NetworkReceiver *)arg;
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
		addr = new IPAddress();
		memcpy(&addr->sin, &clientAddr, addrSize);
		addr->sin.sin_port = htons( port );

		msgReceiver->messageReceived(addr, recBuf, recBytes);
	}
}




bool UDPNetworkManager::connectSender(NetworkAddress *addr)
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




bool UDPNetworkManager::connectReceiver(NetworkAddress *address, NetworkReceiver *receiver)
{
    struct sockaddr_in serverAddr;
//    serverAddr.sin_len = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
#ifdef HAVE_WXWORKS_H
    serverAddr.sin_len = (u_char)sizeof(struct sockaddr_in);
#endif
    serverAddr.sin_port = htons(((IPAddress *)address)->port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int port = ((IPAddress *)address)->port;

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

		//Find the first free (TCPNetworkManager can handle several ports)
	Thread *thread = new Thread();
	UDPServer *udpServer = new UDPServer(this, port, udpSocket);
	thread->start(udpServer, receiver);
	return true;
}


bool UDPNetworkManager::sendMessage(NetworkAddress *addr, char *buf, int bufLen)
{
	int sock = ((IPAddress *)addr)->sock;
	if(sock == -1) //Not connected yet
	{
		if(!connectSender(addr))
			return false; //Still unsuccesful
		sock = ((IPAddress *)addr)->sock;
	}

    if(sendto(sock, buf, (bufLen < MAX_MSG_LEN)?(bufLen):MAX_MSG_LEN, 0, 
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
    return true;
} 




unsigned int UDPNetworkManager::fromNative(unsigned int n)
{
	return htonl(n);
}

unsigned int UDPNetworkManager::toNative(unsigned int n)
{
	return ntohl(n);
}

void UDPNetworkManager::join(IPAddress *addr)
{
    struct ip_mreq ipMreq;

	ipMreq.imr_multiaddr.s_addr = inet_addr(addr->ipAddress);
    ipMreq.imr_interface.s_addr = INADDR_ANY;
    if(setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipMreq, sizeof(ipMreq)) < 0)
    {	
  	   	printf("Error setting socket options in udpStartReceiver\n");
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
    }
}


