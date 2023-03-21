#include "mdsServer.h"
#include <string.h>


/*Conversion ot support charcters not supported by mdsplus:
:  -->  __
.  -->  ___
-  -->  ____
*/
static void convertToPvName(char *in, char *convName)
{
    int len = strlen(in);
    int outLen = 0;
    convName[0] = '\\';
    int idx = 0;
    while(idx < len)
    {
	if(in[idx] == '_')
	{
	    if(idx < len - 1 && in[idx+1] == '_')
	    {
		if(idx < len - 2 && in[idx+2] == '_')
		{
		    if(idx < len - 3 && in[idx+3] == '_')
		    {
			convName[outLen++] = '-';
			idx += 4;
		    }
		    else
		    {
			convName[outLen++] = '.';
			idx += 3;
		    }
		}
		else
		{
		    convName[outLen++] = ':';
		    idx += 2;
		}
	    }
	    else
	    {
		convName[outLen++] = '_';
		idx += 1;
	    }
	}
	else
	{
	    convName[outLen++] = in[idx];
	    idx += 1;
	}
    }
    convName[outLen] = 0;
}		

/*
mdsServer::mdsServer (Tree *tree, bool appendIn) 
{
    append = appendIn;
    mdsPV *pPV;
    mdsPV::initFT();
    
    // pre-create all of the PVs that this server will export
    try {
	int numTags;
	StringArray *tagsArr = tree->findTags("***");
	char **tags = tagsArr->getStringArray(&numTags);
	deleteData(tagsArr);
	for(int i = 0; i < numTags; i++)
	{
printf("TAG: %s\n", tags[i]);
	    char *pvName = new char[strlen(tags[i]) + 2];
//remove \<expName>::
	    int startId;
	    for(startId = 2; startId < strlen(tags[i]); startId++)
	    {
		if(tags[i][startId-1] == ':' && tags[i][startId - 2] ==':')
		    break;
	    } 
	    convertToPvName(&tags[i][startId], pvName);
//	    sprintf(pvName, "%s", &tags[i][startId]);
printf("TAG1: %s\n", pvName);
	    if(!strcmp(pvName, "TOP")) //Alway present to indicate the root
		continue;
	    TreeNode *currNode = tree->getNode(tags[i]);
 	    pPV = new mdsPV(*this, pvName, tree, currNode, append);
	    if(pPV->isValid())
	    	this->installAliasName(*pPV, pvName);
	    else
		delete pPV;
	    delete currNode;
	    delete [] pvName;
	    delete [] tags[i];
	}
	delete [] tags;
    }catch(const MdsException &exc)
    {
	std::cout << "Error reading tags: "<< exc.what() << "\n";
    }
}

*/

mdsServer::mdsServer (Tree *tree, bool appendIn) 
{
    append = appendIn;
    mdsPV *pPV;
    mdsPV::initFT();
    
    // pre-create all of the PVs that this server will export
    try {
	TreeNodeArray *stringNodes = tree->getNodeWild("***", 1 << TreeUSAGE_TEXT);
	int numNodes = stringNodes->getNumNodes();
	for(int i = 0; i < numNodes; i++)
	{
	    char *currName = (*stringNodes)[i]->getNodeName();
/*   A Process Variable is assumed to be defined when at least sibling nodes named PV_NAME and VAL are found
*/	    if(!strcmp(currName, "PV_NAME"))
	    {
		char *fullPath = (*stringNodes)[i]->getFullPath();
		//Replace last PV_NAME with VAL
		sprintf(&fullPath[strlen(fullPath) - strlen("PV_NAME")], "VAL");
		try {
		    TreeNode *valNode = tree->getNode(fullPath);
		    TreeNode *parentNode = (*stringNodes)[i]->getParent();
		    Data *pvNameData = (*stringNodes)[i]->getData();
		    char *pvName = pvNameData->getString();
 	    	    pPV = new mdsPV(*this, pvName, tree, parentNode, append);
	    	    if(pPV->isValid())
		    {
	    		this->installAliasName(*pPV, pvName);
			std::cout << pvName << "\n";
		    }
	    	    else
			delete pPV;
	    	    delete[]pvName;
		    deleteData(pvNameData);
		    delete parentNode;
		    delete valNode;
		}catch(const MdsException &exc)
		{
	
		    std::cout << "Inconsistent node set found: " << fullPath << " " << exc.what() << "\n";
		    //std::cout << "Inconsistent node set found: " << fullPath << " " <<  "\n";
		}
		delete [] fullPath;
	    }
	    delete [] currName;
	}
	delete stringNodes;
     }catch(const MdsException &exc)
    {
	std::cout << "Error Scanning tree: "<< exc.what() << "\n";
    }
 }


mdsServer::~mdsServer()
{
    this->stringResTbl.traverse ( &pvEntry::destroy );
}


// Enters a new alias into the table of PV aliases

void mdsServer::installAliasName(mdsPV &pv, const char *pAliasName)
{
    pvEntry *pEntry;

    pEntry = new pvEntry(pv, *this, pAliasName);
    if (pEntry){
        int resLibStatus;
        resLibStatus = this->stringResTbl.add(*pEntry);
        if(resLibStatus==0){
            return;
        }
        else{
            delete pEntry;
        }
    }
    printf("Unable to enter PV=%s Alias=%s in PV name alias hash table\n", pv.getName(), pAliasName);
}

char *mdsServer::makeUpper(const char *pPVName)
{
    int len = strlen(pPVName);
    char *upName = new char[strlen(pPVName)+1];
    strcpy(upName, pPVName);
    for(int i = 0; i < len; i++)
	upName[i] = toupper(pPVName[i]); 
    return upName;
}
// Advanced version of pvExistTest(), not needed

pvExistReturn mdsServer::pvExistTest(const casCtx & ctx, const caNetAddr &, const char * pPVName)
{
	char *upName = makeUpper(pPVName);
	pvExistReturn status = this->pvExistTest(ctx, (const char *)upName);
	delete [] upName;
	return status;
}

// Checks if a PV with a given alias(a pvEntry) exists on this server

pvExistReturn mdsServer::pvExistTest(const casCtx& ctxIn, const char * pPVName)
{
    char *upName = makeUpper(pPVName);
    stringId id((const char *)upName, stringId::refString);
    pvEntry *pPVE;
    
    pPVE = this->stringResTbl.lookup(id);
    delete [] upName;
    if(!pPVE) return pverDoesNotExistHere;
    
    return pverExistsHere;
}


// Called when a clinet wants to attach to a PV

pvAttachReturn mdsServer::pvAttach(const casCtx &ctx, const char *pPVName)
{ 
    char *upName = makeUpper(pPVName);
    stringId id((const char *)upName, stringId::refString);
    mdsPV *pv;
    pvEntry *pPVE;
	
    //check if the PV with this alias is in the table
    pPVE = this->stringResTbl.lookup(id);
    delete [] upName;
    if(!pPVE){
        return S_casApp_pvNotFound;
    }

    //get the PV from PVEntry
    pv = pPVE->getPV();
    pv->reference();
    return *pv;
}



// Removes an alias from the table of PV aliases

void mdsServer::removeAliasName(pvEntry & entry)
{
    pvEntry * pE;
    pE = this->stringResTbl.remove(entry);
    assert(pE == &entry);
}


