#include "windows.h"
#include "SystemSpecific.h"

class EXPORT SharedMemory
{
	HANDLE hFile; 
	int size;
	static char *startAddress;

public:
	bool initialize(int id, int size);
	char *getStartAddr();
	void dispose(){}
};

