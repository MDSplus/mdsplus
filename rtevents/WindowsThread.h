#include "Lock.h"
#include "Runnable.h"
#include "ThreadAttributes.h"
struct  WithArg{
	Runnable *rtn;
	Lock *lockPtr;
	void *arg;
};
extern  "C" void handlerWithArg(WithArg *);

class Thread
{
	HANDLE threadH;
	Lock lock;
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

