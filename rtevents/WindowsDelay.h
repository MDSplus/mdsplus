#include <Windows.h>
class Delay
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
