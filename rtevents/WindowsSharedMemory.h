#include "Windows.h"

class SharedMemory
{
	HANDLE hFile; 
	int size;
	static char *startAddress;

public:
	bool initialize(int id, int size);
	char *getStartAddr() {return startAddress;}
	void dispose(){}
};

