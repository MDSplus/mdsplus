#include <treeshr.h>
#include "treeshrp.h"
#include <ncidef.h>
int ConnectTreeRemote(PINO_DATABASE *dblist, char *tree, char *subtree_list,int status)
{
	return status;
}

int SetStackSizeRemote(PINO_DATABASE *dblist, int stack_size)
{
	return 1;
}

int CloseTreeRemote(PINO_DATABASE *dblist, int call_hook)
{
	return 1;
}

int GetNciRemote(PINO_DATABASE *dblist, int nid_in, struct nci_itm *nci_itm)
{
	return 1;
}

int GetRecordRemote(PINO_DATABASE *dblist, int nid_in, struct descriptor_xd *dsc)
{
	return 1;
}

int FindNodeRemote(PINO_DATABASE *dblist, char *path, int *outnid)
{
	return 1;
}

int FindNodeWildRemote(PINO_DATABASE *dblist, char *path, int *nid_out, void **ctx_inout, int usage_mask)
{
	return 1;
}

int FindNodeEndRemote(PINO_DATABASE *dblist, void **ctx_in)
{
	return 1;
}

char *FindNodeTagsRemote(PINO_DATABASE *dblist, int nid_in, void **ctx_ptr)
{
	return NULL;
}

char *AbsPathRemote(PINO_DATABASE *dblist, char *inpath)
{
	return NULL;
}

int SetDefaultNidRemote(PINO_DATABASE *dblist, int nid)
{
	return 1;
}

int GetDefaultNidRemote(PINO_DATABASE *dblist, int *nid)
{
	return 1;
}
