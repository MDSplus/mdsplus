#include <windows.h>
#include "SystemSpecific.h"
class EXPORT Delay
{
	long milliSecs;
public:
	Delay(int milliSecs)
	{
		this->milliSecs = milliSecs;
	}
	void wait()
	{
		Sleep(milliSecs);
	}
};
