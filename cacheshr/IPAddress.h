#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H


#ifdef HAVE_WINDOWS_H
#include <winsock.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <string.h>

class IPAddress:public ChannelAddress
{

	char addressStr[512];

	void buildAddress()
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
		struct sockaddr_in sin;
		memset((char *)&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		struct hostent *hp = NULL;
		hp = gethostbyname(ipAddress);
		if (hp == NULL)
		{
			int addr = inet_addr(ipAddress);
			if (addr != 0xffffffff)
    			hp = gethostbyaddr((const char *) &addr, (int) sizeof(addr), AF_INET);
		}
		memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
		sin.sin_port = htons( port );
	}

public:	
	static bool initialized;
	int socket;
	int port;
	struct sockaddr_in sin;

	char ipAddress[512];
	
	IPAddress(int port)
	{
		socket = -1;
		strcpy(ipAddress, "localhost");
		this->port = port;
		buildAddress();
	}
	IPAddress()
	{
		socket = -1;
	}

	IPAddress(char *ip, int port)
	{
		socket = -1;
		strcpy(ipAddress, ip);
		this->port = port;
		buildAddress();
	}

	IPAddress(char *addr)
	{
		setAddressString(addr);
		buildAddress();
	}

	char *getAddressString()
	{
		return addressStr;
	}
	
	//Adresses are of the form <IP address>:<port>
	void setAddressString(char *addrStr)
	{
		strcpy(addressStr, addrStr);
		int colonIdx;
		int len = strlen(addressStr);
		for(colonIdx = 0;colonIdx < len && addressStr[colonIdx] != ':'; colonIdx++);
		if(colonIdx < len - 1)
		{
			addressStr[colonIdx] = 0;
			strcpy(ipAddress, addressStr);
			addressStr[colonIdx] = ':';
			sscanf(&addressStr[colonIdx + 1], "%d", &port);
		}
	}
};

#endif
