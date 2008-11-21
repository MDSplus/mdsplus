#ifndef EXIT_HANDLER_H
#define EXIT_HANDLER_H
#include "Runnable.h"

class ExitHandler
{
public:
	static Runnable *handler;
	static void atExit(Runnable *inHandl);
	static void dispose();
};

#endif
