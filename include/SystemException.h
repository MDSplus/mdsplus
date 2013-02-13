#ifndef SYSTEMEXCEPTION_H_
#define SYSTEMEXCEPTION_H_
#include "SystemSpecific.h"
#include <string.h>
#include <stdio.h>
#ifdef HAVE_WINDOWS_H
#include "WindowsSystemException.h"
#else
class EXPORT SystemException
{
	char msg[512];

public:
	SystemException(const char *msg, int errNo)
	{
		perror("ERRORE DI SISTEMA");
		sprintf(this->msg, "%s %d", msg, errNo);
		this->msg[255] = 0;
	}
	char *what()
	{
		return msg;
	}
};
#endif
#endif /*SYSTEMEXCEPTION_H_*/
