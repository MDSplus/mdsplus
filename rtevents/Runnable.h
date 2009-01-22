#ifndef RUNNABLE_H_
#define RUNNABLE_H_
#include "SystemSpecific.h"
class EXPORT Runnable
{
public: 
	virtual void run(void *arg) = 0;
};
#endif /*RUNNABLE_H_*/
