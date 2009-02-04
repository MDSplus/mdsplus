#include "SharedMemManager.h"
#include "TCPNetworkManager.h"
#include "IPAddress.h"

static int readFromSocket(int sock, int numBytes, char *buf)
{
	int numReadBytes = 0;
	int currBytes;

	while(numReadBytes < numBytes)
	{
		if((currBytes = recv(sock, &buf[numReadBytes], numBytes - numReadBytes,0)) <= 0)
		{
			printf("ERRORE IN recv\n");
			printf("Error in recv: socket closed\n");
			return -1;
		}
			printf("recv OK\n");
		numReadBytes += currBytes;
	}
	return 0;
}


void TCPHandler::run(void *arg)
{
//TCP Message protocol: length (4 bytes), message (length bytes) 

	NetworkReceiver *msgReceiver = (NetworkReceiver *)arg;
	IPAddress currAddr(&addr->sin, addr->port, -1);
	while(true)
	{
		int len;
		unsigned int readLen;
		int sock = addr->getSocket();
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, NULL, 0);
		if(readFromSocket(sock, 4, (char *)&readLen) == -1)
		{
			printf("Socket communication terminated\n");
			msgReceiver->connectionTerminated((NetworkAddress *)&currAddr);
			return;
		}
		len = ntohl(readLen);
		char *buf = new char[len];
		if(readFromSocket(sock, len, buf) == -1)
		{
			printf("Socket communication terminated\n");
			msgReceiver->connectionTerminated((NetworkAddress *)&currAddr);
			return;
		}
		printf("Message received\n");
		msgReceiver->messageReceived((NetworkAddress *)&currAddr, buf, len);
		delete [] buf;
	}
}




void TCPServer::run(void *arg)
{
	NetworkReceiver *msgReceiver = (NetworkReceiver *)arg;
	while(true)
	{
		struct sockaddr clientAddr;
		memset(&clientAddr, 0, sizeof(clientAddr));
#ifdef HAVE_WINDOWS_H
		int addrSize = sizeof(clientAddr);
		printf("ATTENDO IN ACCEPT\n");
		int newSocket = accept(sock, &clientAddr, &addrSize);
		printf("RICEVUTO ACCEPT\n");
		((sockaddr_in *)&clientAddr)->sin_port = htons(port);
		printf("New Connection received\n");
		if(sock == INVALID_SOCKET)
#else
		socklen_t addrSize = sizeof(clientAddr);
		int newSocket = accept(sock, &clientAddr, &addrSize);
		((sockaddr_in *)&clientAddr)->sin_port = htonl(port);
		printf("New Connection received\n");
		if(newSocket == -1)
#endif
		{   
			printf("Cannot accept socket \n");
			return;
		}
		
		TCPHandler *tcpHandler = new TCPHandler(manager, new IPAddress((struct sockaddr_in *)&clientAddr, port, newSocket));
		thread.start((Runnable *)tcpHandler, (void *)msgReceiver);
	}
}


bool TCPNetworkManager::connectSender(NetworkAddress *addr)
{
	IPAddress *tcpAddr = (IPAddress *)addr;
	tcpAddr->sock= socket(AF_INET, SOCK_STREAM, 0);
#ifdef HAVE_WINDOWS_H
	if (tcpAddr->sock == INVALID_SOCKET) return false;
#else
	if (tcpAddr->sock == -1) return false;
#endif
	printf("CONNETING TO %s port %d\n", tcpAddr->ipAddress, tcpAddr->port);
	if(connect(tcpAddr->sock, (struct sockaddr *)&tcpAddr->sin, sizeof(tcpAddr->sin)))
	{
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
		char buf[512];
		sprintf(buf, "Error Connecting to %s", addr->getAddressString());
		tcpAddr->sock = -1;
		return false;
	}
	setsockopt(tcpAddr->sock, IPPROTO_TCP, TCP_NODELAY, NULL, 0);
	printf("CONNECTED\n");
	return true;

}


bool TCPNetworkManager::connectReceiver(NetworkAddress *address, NetworkReceiver *receiver)
{
	int port = ((IPAddress *)address)->port;
	int tcpSocket;

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
#ifdef HAVE_WINDOWS_H
	inAddress.sin_addr.s_addr = INADDR_ANY;
	if(bind(tcpSocket, (SOCKADDR *)&inAddress, addrSize) != 0)
#else
	inAddress.sin_addr.s_addr = INADDR_ANY;
	if(bind(tcpSocket, (struct sockaddr *)&inAddress, addrSize) != 0)
#endif
	{   
		printf("Cannot bind socket at port %d\n", port);
		return false;
	}

	if(listen(tcpSocket, 5) != 0)
	{   
		printf("Cannot listen socket at port %d\n", port);
		return false;
	}

	//Start a new Server thread listening to incoming connections.
	//Find the first free (TCPNetworkManager can handle several ports)
	Thread *thread = new Thread();
	TCPServer *tcpServer = new TCPServer(this, tcpSocket, port);
	thread->start(tcpServer, receiver);
	return true;
}

bool TCPNetworkManager::sendMessage(NetworkAddress *addr, char *buf, int bufLen)
{
	int sock = ((IPAddress *)addr)->sock;

	if(sock == -1) //Not connected yet
	{
		if(!connectSender(addr))
			return false; //Still unsuccesful
		sock = ((IPAddress *)addr)->sock;
	}
	int convLen =  htonl(bufLen);
	int sent = 	send(sock, (char *)&convLen, sizeof(int), 0); 
	printf("Sent Bytes: %d\n", sent);
	if(sent == -1) //If connection meanwhile went down
	{
		if(!connectSender(addr))
			return false; //Still unsuccesful
		sock = ((IPAddress *)addr)->sock;
		if(send(sock, (char *)&convLen, sizeof(int), 0) == -1)
			return false;
	}
	send(sock, buf, bufLen, 0);
	return true;
}

unsigned int TCPNetworkManager::fromNative(unsigned int n)
{
	return htonl(n);
}

unsigned int TCPNetworkManager::toNative(unsigned int n)
{
	return ntohl(n);
}


bool IPAddress::initialized;
