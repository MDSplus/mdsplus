#include "RelativePointer.h"

bool operator > (void *ptr, RelativePointer &rp)
{
	return ptr > rp.getAbsAddress();
}

bool operator < (void *ptr, RelativePointer &rp)
{
	return ptr < rp.getAbsAddress();
}

bool operator > (RelativePointer &rp, void *ptr)
{
	return rp.getAbsAddress() > ptr;
}

bool operator < (RelativePointer &rp, void *ptr)
{
	return rp.getAbsAddress() < ptr;
}

bool operator > (RelativePointer &rp1, RelativePointer &rp2)
{
	return rp1.getAbsAddress() > rp2.getAbsAddress();
}
bool operator < (RelativePointer &rp1, RelativePointer &rp2)
{
	return rp1.getAbsAddress() < rp2.getAbsAddress();
}

bool operator == (void *ptr, RelativePointer &rp)
{
	return ptr == rp.getAbsAddress();
}
bool operator == (RelativePointer &rp, void *ptr)
{
	return ptr == rp.getAbsAddress();
}
bool operator == (RelativePointer &rp1, RelativePointer &rp2)
{
	return rp1.getAbsAddress() == rp2.getAbsAddress();
}
void * operator + (int i, RelativePointer &rp)
{
	return i + (char *)rp.getAbsAddress();
}
void * operator + (RelativePointer &rp, int i)
{
	return i + (char *)rp.getAbsAddress();
}
