#include "mdsServer.h"



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
printf("Trovato %d tags\n", numTags);
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
	    sprintf(pvName, "%s", &tags[i][startId]);
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
    }catch(MdsException *exc)
    {
	cout << "Error reading tags: "<< exc->what() << "\n";
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


