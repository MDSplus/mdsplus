#ifndef THREADATTRIBUTES_H_
#define THREADATTRIBUTES_H_
#define CPU_MASK_SIZE 256
class ThreadAttributes
{
	int stackSize;
	int priority;
	int cpuMask[CPU_MASK_SIZE];
};

#endif /*THREADATTRIBUTES_H_*/
