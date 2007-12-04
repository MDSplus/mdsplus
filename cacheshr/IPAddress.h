#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H


#ifdef HAVE_WINDOWS_H
#include <winsock.h>
#include <windows.h>
#else
#ifdef HAVE_VXWORKS_H
#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <hostlib.h>
#include <stdio.h>
#include <semLib.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#endif

#include <string.h>
#include "ChannelAddress.h"

class IPAddress:public ChannelAddress
{


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
public:	
	char addressStr[512];
	static bool initialized;
	int sock;
	int port;
	struct sockaddr_in sin;

	char ipAddress[512];
	
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
	void print()
	{
	    printf("%s\t%d\t%x\n", &ipAddress[0], port, sin.sin_addr);
	}
};

#endif
