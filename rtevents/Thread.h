#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include "Runnable.h"

struct  WithArg{
	Runnable *rtn;
	void *arg;
};
extern  "C" void handlerWithArg(WithArg *);

class Thread
{
	pthread_t thread;
public:
	void start(Runnable *rtn, void *arg = 0);
	void join();
	friend void handlerWithArg(WithArg *rtn);
};




#endif /*THREAD_H_*/
