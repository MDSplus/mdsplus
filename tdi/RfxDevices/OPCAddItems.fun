Public fun OPCAddItems(in _serverHandle, in _gName, in _items, in _nItem)
{
    _status = opcAccess->OPCAddItems( val( _serverHandle ) , _gName, _items, val(_nItem));
    return ( _status );
}