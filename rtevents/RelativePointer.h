#ifndef RELATIVEPOINTER_H_
#define RELATIVEPOINTER_H_
#include "SystemSpecific.h"
#ifndef NULL
#define NULL 0
#endif
//Class RelativePointer handles memory references stored as a 64 bit offset from the 
//actual, process-specific, address of the current instance
/*#ifndef HAVE_WINDOWS_H
typedef long long _int64;
typedef unsigned long long _int64u;
#define EXPORT
#else
#define EXPORT __declspec(dllexport)
#endif
*/

class EXPORT RelativePointer
{
	_int64 offset;

public:

	void *getAbsAddress()
	{
		if(!offset)
			return 0;
		return reinterpret_cast<void *>(offset + reinterpret_cast<_int64>(this));
	}
	void * operator = (void *ptr)
	{
//Address 0 is treated in a special way, since it has a special meaning
//otherwise, a 0 saved in a memory space will not be retrieved as 0 in another memory space
		if(!ptr)
			offset = 0;
		else
			offset = reinterpret_cast<_int64>(ptr) - reinterpret_cast<_int64>(this);
		return ptr;
	}
	void * operator = (RelativePointer &rp)
	{
//Address 0 is treated in a special way, since it has a special meaning
//otherwise, a 0 saved in a memory space will not be retrieved as 0 in another memory space
		void *ptr = rp.getAbsAddress();
		*this = ptr;
		return ptr;
	}
};



#endif /*RELATIVEPOINTER_H_*/
