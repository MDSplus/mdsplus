#include "mdsServer.h"


void mdsChannel::setOwner(const char * const /* pUserName */, const char * const /* pHostName */)
{
}


bool mdsChannel::readAccess() const
{
    return true;
}


bool mdsChannel::writeAccess() const
{
    return true;
}


