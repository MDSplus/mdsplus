#ifndef THREAD_H
#define THREAD_H
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include "Runnable.h"

extern "C" void handler(Runnable *);

class Thread
{
#ifdef HAVE_WINDOWS_H
	HANDLE threadH;
#endif
public:
	void start(Runnable *rtn);

	friend void handler(Runnable *rtn);
};

#endif

