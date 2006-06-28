#ifndef TCP_ADDRESS_H
#define TCP_ADDRESS_H

#include <string.h>

class TCPAddress:public ChannelAddress
{

	char addressStr[512];

public:	
	int socket;
	int port;
	char ipAddress[512];
	
	TCPAddress(int port)
	{
		socket = -1;
		strcpy(ipAddress, "localhost");
		this->port = port;
	}
	TCPAddress()
	{
		socket = -1;
	}

	TCPAddress(char *ip, int port)
	{
		socket = -1;
		strcpy(ipAddress, ip);
		this->port = port;
	}

	TCPAddress(char *addr)
	{
		setAddressString(addr);
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