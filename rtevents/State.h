#ifndef STATE_H_
#define STATE_H_

class State
{
	bool isBigEndian()
	{
		char *c;
		short s = 1;
		c = (char *)&s;
		return c[1] != 0;
	}

	void swap(char &a, char &b)
	{
		char c;
		c = a;
		a = b;
		b = c;
	}
public:
	int swap(int n)
	{
		if(isBigEndian()) return n;
		char a[4];
		memcpy(a, &n, 4);
		swap(a[0], a[3]);
		swap(a[1], a[2]);
		return *((int *)a);
	}
	_int64 swap(_int64 n)
	{
		if(isBigEndian()) return n;
		char a[8];
		memcpy(a, &n, 8);
		swap(a[0], a[7]);
		swap(a[1], a[6]);
		swap(a[2], a[5]);
		swap(a[3], a[4]);
		return *((_int64 *)a);
	}
};

#endif