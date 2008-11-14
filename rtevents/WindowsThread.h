#include "Lock.h"
#include "Runnable.h"
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
public:
	void start(Runnable *rtn, void *arg = 0);
	void join();
	friend void handlerWithArg(WithArg *rtn);
};

