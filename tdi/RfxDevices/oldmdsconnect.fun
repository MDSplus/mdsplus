public fun oldmdsconnect(in _ip, in _port)
{

    write(*, 'oldmdsconnect');	
    return(OldMdsAccess->mdsconnect(_ip, _port));
}
