#ifndef  CALLBACK_MANAGER_H
#define CALLBACK_MANAGER_H
#include "SharedMemNodeData.h"
#include "NotifyClient.h"
class CallbackManager
{
	long next, prev;
	NotifyClient notifyClient;

public:
	void setNext(void *nxt)
	{
		next = (long)nxt - (long)this;
	}
	void setPrev(void *prv)
	{
		prev = (long)prv - (long)this;
	}

	void setNotify(NotifyClient *notify)
	{
		this->notifyClient = *notify;
	}
	
	CallbackManager *getNext()
	{
		return (CallbackManager *)((char *)this + next);
	}
	CallbackManager *getPrev()
	{
		return (CallbackManager *)((char *)this + prev);
	}



	void callCallback()
	{
		notifyClient.notify();
		if(!notifyClient.isMulticast()) //individual notification only if multicast notification is not supported
		{
			CallbackManager *nxt = getNext();
			if(nxt)
				nxt->callCallback();
		}
	}

};
#endif