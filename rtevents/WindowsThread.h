#include "SystemSpecific.h"
#include "Runnable.h"
#include "ThreadAttributes.h"
#include <Windows.h>
struct  WithArg{
	Runnable *rtn;
	HANDLE sem; 
	void *arg;
};
extern  "C" void handlerWithArg(WithArg *);

class EXPORT Thread
{
	HANDLE threadH;
	HANDLE semH;
	WithArg *withArg;
	ThreadAttributes *attributes;
public:
	Thread()
	{
		attributes = NULL;
	}
	Thread(ThreadAttributes *attr)
	{
		if(attr)
		{
			attributes = new ThreadAttributes;
			*attributes = *attr;
		}
		else
			attributes = NULL;
	}
	~Thread()
	{
		if(attributes)
			delete attributes;
		CloseHandle(semH);
	}
	void setAttributes(ThreadAttributes *attr)
	{
		attributes = new ThreadAttributes;
		*attributes = *attr;
	}
	void start(Runnable *rtn, void *arg = 0);
	void join();
	friend void handlerWithArg(WithArg *rtn);
};

