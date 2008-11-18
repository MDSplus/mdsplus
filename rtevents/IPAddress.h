#ifndef IPADDRESS_H_
#define IPADDRESS_H_
#include "NetworkAddress.h"
#include <string.h>

#ifdef HAVE_WINDOWS_H
#include <winsock.h>
#include <windows.h>
#include <stdio.h>
#else

#ifdef HAVE_VXWORKS_H
#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <hostlib.h>
#include <stdio.h>
#include <semLib.h>
#else
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#endif


class IPAddress:public NetworkAddress
{
	friend class TCPMessageManager;

public:	
	static bool initialized;
	int sock;
	int port;
	struct sockaddr_in sin;
	char ipAddress[512];
	


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
#ifdef HAVE_VXWORKS_H
		bzero((char *)&sin, sizeof(struct sockaddr_in));
		
		sin.sin_family = AF_INET;
		sin.sin_port = htons( port );
		sin.sin_len = (u_char)(sizeof(struct sockaddr_in));

      		if(((sin.sin_addr.s_addr = inet_addr(ipAddress)) == ERROR) &&
	     		((sin.sin_addr.s_addr = hostGetByName(ipAddress)) == ERROR))  

	    		printf("Unknown recipient name in IP address  initialization\n");
#else
		memset((char *)&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		struct hostent *hp =(struct hostent *) NULL;
		hp = gethostbyname(ipAddress);
		if (hp == NULL)
		{
			int addr = inet_addr(ipAddress);
			if (addr != 0xffffffff)
    			hp = gethostbyaddr((const char *) &addr, (int) sizeof(addr), AF_INET);
		}
		memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
		sin.sin_port = htons( port );
#endif
	}
	//Adresses are of the form <IP address>:<port>

	
	

public:	
	IPAddress(int port)
	{
		sock = -1;
		strcpy(ipAddress, "localhost");
		this->port = port;
		buildAddress();
	}
	IPAddress()
	{
		sock = -1;
		ipAddress[0] = 0;
		memset(&sin, 0, sizeof(struct sockaddr_in));
	}
	IPAddress(char *ip, int port)
	{
		sock = -1;
		strcpy(ipAddress, ip);
		this->port = port;
		buildAddress();
	}

	IPAddress(char *addr)
	{
		sock = -1;
		setAddressString(addr);
		buildAddress();
	}
	IPAddress(IPAddress *inAddr)
	{
		memcpy(&sin, &inAddr->sin, sizeof(sin));
		port = inAddr->port;
		sin.sin_port = htons(port);
		ipAddress[0] = 0;
		sock = inAddr->sock;
	}
	IPAddress(struct sockaddr_in *inSin, int inPort, int inSock) 
	{
		sin = *inSin;
		sock = inSock;
		port = inPort;
		ipAddress[0] = 0;
	}

	virtual bool equals(NetworkAddress *addr)
	{
		IPAddress *ipAddr = (IPAddress *)addr;
		return sin.sin_addr.s_addr == ipAddr->sin.sin_addr.s_addr;
	}
	
	virtual char *getAddressString()
	{
		if(!ipAddress[0])
		{
#ifdef HAVE_WINDOWS_H
			struct hostent *host = gethostbyaddr((const char *)&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
#else
			struct hostent *host = gethostbyaddr(&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
#endif
			if(host && host->h_name)
			{
				strcpy(ipAddress, host->h_name);
			}
		}
		return ipAddress;
	}
	virtual void setAddressString(char *addrStr)
	{
		int colonIdx;
		int len = strlen(addrStr);
		for(colonIdx = 0;colonIdx < len && addrStr[colonIdx] != ':'; colonIdx++);
		if(colonIdx < len - 1)
		{
			addrStr[colonIdx] = 0;
			strcpy(ipAddress, addrStr);
			addrStr[colonIdx] = ':';
			sscanf(&addrStr[colonIdx + 1], "%d", &port);
		}
	}

	
	int getSocket() {return sock;}
	int getPort() { return port;}
	void print()
	{
	    printf("%s\t%d\t%x\n", &ipAddress[0], port, sin.sin_addr);
	}
};


#endif /*IPADDRESS_H_*/
