public fun OPCGet(in _serverHandle, in _item)
{
    _ris = 0;
    _status = opcAccess->OPCGet(  val( _serverHandle ), _item, xd(_ris));
    return( _ris );
}
