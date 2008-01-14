Public fun OPCRead(in _serverHandle, in _items, in _nItem)
{
	_val = 0;
    _status = opcAccess->OPCRead(  val( _serverHandle ), _items, val(_nItem), xd(_val) );
    return ( _val );
}