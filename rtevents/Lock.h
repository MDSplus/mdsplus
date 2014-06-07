#ifndef MDS_LOCK_H_
#define MDS_LOCK_H_

#include "SystemSpecific.h"
#include "UnnamedSemaphore.h"
#include "SystemException.h"
#include "RelativePointer.h"

//Class Lock handles a critical section using a unnamed semaphore
class EXPORT Lock
{
public:
	//Initialize the semaphore structure. If the semaphore is shared among processes
	//(i.e. the object instance is located in shared memory, initialization is to 
	//be carried out only once.
	void initialize() {
		sem.initialize(1);
	}
	

	void dispose() {
		sem.dispose();
	}
	
	
	void lock() {
		try {
			sem.wait();
		} catch(SystemException *exc) {
			sem.initialize(0);
		}
	}

	void unlock() {
		sem.post();
	}

private:
	UnnamedSemaphore sem;
};

#endif /* MDS_LOCK_H_ */
