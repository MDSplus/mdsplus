#include "mdsServer.h"


pvEntry::pvEntry(mdsPV  & pvIn, mdsServer & casIn, const char * pAliasName) : 
    stringId(pAliasName),
    pv(pvIn),
    cas(casIn) 
{
    assert ( this->stringId::resourceName() != NULL );
}

pvEntry::~pvEntry()
{
    this->cas.removeAliasName(*this);
}
mdsPV * pvEntry::getPV()
{
    return &pv;
}

void pvEntry::destroy()
{
    delete this;
}
