#ifndef TREEDESCRIPTOR_H
#define TREEDESCRIPTOR_H

#include <string.h>

class TreeDescriptor {
	char name[64];
	int shot;
public:
	TreeDescriptor()
	{
		memset(name, 0, 64);
		shot = -1;
	}
	TreeDescriptor(char *name, int shot)
	{
		strncpy(this->name, name, 63);
		this->name[63] = 0;
		this->shot = shot;
	}
	int getShot() {return shot;}
	char *getName() {return name;}
	bool operator == (TreeDescriptor treeDesc)
	{
		if(shot != treeDesc.shot)
			return false;
		if(strcmp(name, treeDesc.name))
			return false;
		return true;
	}
	bool operator > (TreeDescriptor treeDesc)
	{
		if(shot != treeDesc.shot)
			return shot > treeDesc.shot;
		for(int i = 0; i < 64; i++)
		{
			if(!name[i] && !treeDesc.name[i]) return false;
			if(!name[i] && treeDesc.name[i]) return false;
			if(!treeDesc.name[i] && name[i]) return true;
			if(name[i] != treeDesc.name[i])
				return name[i] > treeDesc.name[i];
		}
		return false;
	}
	bool operator < (TreeDescriptor treeDesc)
	{
		if(shot != treeDesc.shot)
			return shot < treeDesc.shot;
		for(int i = 0; i < 64; i++)
		{
			if(!name[i] && !treeDesc.name[i]) return false;
			if(!name[i] && treeDesc.name[i]) return true;
			if(!treeDesc.name[i] && name[i]) return false;
			if(name[i] != treeDesc.name[i])
				return name[i] < treeDesc.name[i];
		}
		return false;
	}
};

#endif