#ifndef SIMPLE_ALLOCATION_MANAGER 
#define SIMPLE_ALLOCATION_MANAGER
#include <stdlib.h>
class SimpleAllocationManager
{

public:
	//Initialize structures. To be called only once by the first process creating the shared memory segment
	void initialize(int size){}
	char *allocateShared(int size)
	{
		return (char *)malloc(size);
	}
	//Free buffer
	void deallocateShared(char *addr, int size)
	{
		free((void *)size);
	}
	

};

#endif
