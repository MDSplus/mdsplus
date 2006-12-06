#ifndef  CALLBACK_MANAGER_H
#define CALLBACK_MANAGER_H
#include "Notifier.h"

class CallbackManager
{
	long next, prev;
	Notifier notifier;

public:
	void setNext(void *nxt)
	{
		if(nxt == NULL)
			next = 0;
		else
			next = (long)nxt - (long)this;
	}
	void setPrev(void *prv)
	{
		if(prv == NULL)
			prev = 0;
		else 
			prev = (long)prv - (long)this;
	}

	CallbackManager *getNext()
	{
		if(next == 0) return NULL;
		return (CallbackManager *)((char *)this + next);
	}
	CallbackManager *getPrev()
	{
		if(prev == 0) return NULL;
		return (CallbackManager *)((char *)this + prev);
	}

	void initialize(int nid, void (*callback)(int))
	{
	    notifier.initialize(nid, callback);
	}

	

	void dispose()
	{
		notifier.dispose();
	}
	void callCallback()
	{
		notifier.notify();
		if(!notifier.isMulticast()) //individual notification only if multicast notification is not supported
		{
			CallbackManager *nxt = getNext();
			if(nxt)
				nxt->callCallback();
		}
	}

};
#endif
