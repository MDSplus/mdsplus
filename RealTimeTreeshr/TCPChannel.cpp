#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#endif


#include "TCPChannel.h"
bool TCPChannel::initialized;

struct TCPInfo{
	TCPChannel *channel;
	int socket;
};

#ifdef HAVE_WINDOWS_H


static int readFromSocket(int sock, int numBytes, char *buf)
{
	int numReadBytes = 0;
	int currBytes;

	while(numReadBytes < numBytes)
	{
		if((currBytes = recv(sock, &buf[numReadBytes], numBytes - numReadBytes,0)) < 0)
		{
			printf("Error in recv: socket closed\n");
			return -1;
		}
		numReadBytes += currBytes;
	}
	return 0;
}


extern "C" void handleTCP(TCPInfo *info)
{
//TCP Message protocol: type (1 byte), length (4 bytes), message (length bytes) 

	TCPAddress addr;
	addr.socket = info->socket;
	while(true)
	{
		char type;
		int len;
		if(readFromSocket(info->socket, 1, &type) == -1)
		{
			delete info;
			printf("Socket communication terminated\n");
			return;
		}
		unsigned int readLen;
		if(readFromSocket(info->socket, 4, (char *)&readLen) == -1)
		{
			delete info;
			printf("Socket communication terminated\n");
			return;
		}
		len = ntohl(readLen);
		char *buf = new char[len];
		if(readFromSocket(info->socket, len, buf) == -1)
		{
			delete info;
			printf("Socket communication terminated\n");
			return;
		}
		info->channel->messageReceived(&addr, type, buf, len);
	}
}




extern "C" void serveTCP(TCPInfo *info)
{
	while(true)
	{
		int socket = accept(info->socket, NULL, NULL);
		if(socket == INVALID_SOCKET)
		{   
			printf("Cannot accept socket \n");
			return;
		}
		TCPInfo *currInfo = new TCPInfo;
		currInfo->channel = info->channel;
		currInfo->socket = socket;
		_beginthread((void (__cdecl *)(void *))handleTCP, 0, (void *)currInfo);
	}
}



TCPChannel::TCPChannel()
{
    WSADATA wsaData;
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(1,1);

	if(!initialized)
	{
		initialized = true;
		WSAStartup(wVersionRequested,&wsaData);
	}
}

bool TCPChannel::connectSender(ChannelAddress *addr)
{
	TCPAddress *tcpAddr = (TCPAddress *)addr;
	tcpAddr->socket= socket(AF_INET, SOCK_STREAM, 0);
	if (tcpAddr->socket == INVALID_SOCKET) return false;

/*	struct hostent *hp = NULL;
	hp = gethostbyname(tcpAddr->ipAddress);
	if (hp == NULL)
	{
		int addr = inet_addr(tcpAddr->ipAddress);
		if (addr != 0xffffffff)
    		hp = gethostbyaddr((const char *) &addr, (int) sizeof(addr), AF_INET);
	}
	
	
	struct sockaddr_in sin;
	sin.sin_port = tcpAddr->port;
	sin.sin_family = AF_INET;

	int intAddr = inet_addr(tcpAddr->ipAddress);
	memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
*/

	struct sockaddr_in sin;
	memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
	struct hostent *hp = NULL;
	hp = gethostbyname(tcpAddr->ipAddress);
	if (hp == NULL)
	{
		int addr = inet_addr(tcpAddr->ipAddress);
		if (addr != 0xffffffff)
    		hp = gethostbyaddr((const char *) &addr, (int) sizeof(addr), AF_INET);
	}
	memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
    //sin.sin_addr.s_addr = inet_addr( tcpAddr->ipAddress );
    sin.sin_port = htons( tcpAddr->port );




	printf("ADESSO MI CONNETTO A %s port %d\n", tcpAddr->ipAddress, tcpAddr->port);
	if(connect(tcpAddr->socket, (struct sockaddr *)&sin, sizeof(sin)))
	{
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

		return false;
	}
	printf("CONNESSO\n");
	return true;






}

bool TCPChannel::connectReceiver(ChannelAddress *address)
{
	char *ipAddress = ((TCPAddress *)address)->ipAddress;
	int port = ((TCPAddress *)address)->port;


	int addrSize = sizeof(struct sockaddr_in), retAddrSize = 0;
	struct sockaddr_in inAddress, retAddress;
	if((tcpSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		printf("Error creating socket\n");
		return false;
	}
	memset((char *)&inAddress, 0, addrSize);
	memset((char *)&retAddress, 0, addrSize);
	inAddress.sin_family = AF_INET;
	inAddress.sin_port = htons(port);
	inAddress.sin_addr.s_addr = ADDR_ANY;

	if(bind(tcpSocket, (SOCKADDR *)&inAddress, addrSize) != 0)
	{   
		printf("Cannot bind socket at port %d\n", port);
		return false;
	}

	if(listen(tcpSocket, 5) != 0)
	{   
		printf("Cannot listen socket at port %d\n", port);
		return false;
	}
	TCPInfo *info = new TCPInfo;
	info->channel = this;
	info->socket = tcpSocket;

	_beginthread((void (__cdecl *)(void *))serveTCP, 0, (void *)info);
	return true;
}


bool TCPChannel::sendMessage(ChannelAddress *addr, char *buf, int bufLen, char type)
{
	int socket = ((TCPAddress *)addr)->socket;

	if(socket == -1) //Not connected yet
		connectSender(addr);
	if(socket == -1) //Still unsuccesful
		return false;

	int convLen = fromNative(bufLen);
	send(socket, &type, 1, 0);
	send(socket, (char *)&convLen, sizeof(int), 0); 
	send(socket, buf, bufLen, 0);
	return true;
}

char *TCPChannel::receiveMessage(ChannelAddress *addr, int *retLen, char *retType)
{
	int socket = ((TCPAddress *)addr)->socket;
	if(readFromSocket(socket, 1, retType) == -1)
		return 0;

	unsigned int readLen;
	if(readFromSocket(socket, sizeof(int), (char *)&readLen) == -1)
		return 0;
	*retLen = ntohl(readLen);
	char *buf = new char[*retLen];
	if(readFromSocket(socket, *retLen, buf) == -1)
	{
		delete [] buf;
		return 0;
	}
	return buf;
}


unsigned int TCPChannel::fromNative(int n)
{
	return htonl(n);
}

int TCPChannel::toNative(unsigned int n)
{
	return ntohl(n);
}


#endif //HAVE_WINDOWS_H