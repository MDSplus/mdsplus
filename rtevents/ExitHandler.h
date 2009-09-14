#ifndef EXIT_HANDLER_H
#define EXIT_HANDLER_H
#include "SystemSpecific.h"
#include "Runnable.h"
#include "Lock.h"

class EXPORT ExitHandler
{
public:

	static int handlerIdx;
	static void atExit(Runnable *inHandl);
	static void dispose();
};
#endif
