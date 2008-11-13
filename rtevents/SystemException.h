#ifndef SYSTEMEXCEPTION_H_
#define SYSTEMEXCEPTION_H_
#include <string.h>
#include <stdio.h>
class SystemException
{
	char msg[512];

public:
	SystemException(char *msg, int errNo)
	{
		perror("ERRORE DI SISTEMA");
		char sysMsg[256];
		strerror_r(errNo, sysMsg, 256);

		sprintf(this->msg, "%s %s", msg, sysMsg);
		this->msg[255] = 0;
	}
	char *what()
	{
		return msg;
	}
};

#endif /*SYSTEMEXCEPTION_H_*/
