#ifndef THREADATTRIBUTES_H_
#define THREADATTRIBUTES_H_
#define CPU_MASK_SIZE 4
class ThreadAttributes
{
public:
	int stackSize;
	int priority;
	int policy;
	int cpuMask[CPU_MASK_SIZE];
};

#endif /*THREADATTRIBUTES_H_*/
