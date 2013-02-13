#include <Windows.h>
#include "SystemSpecific.h"

class EXPORT SystemException
{
	char msg[1024];

public:
	SystemException(char *msg, int errNo)
	{
		char sysMsg[512];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, errNo, 0, (LPSTR)sysMsg, 512, 0);
		sprintf(this->msg, "%s %s", msg, sysMsg);
		this->msg[1023] = 0;
	}
	char *what()
	{
		return msg;
	}
};
