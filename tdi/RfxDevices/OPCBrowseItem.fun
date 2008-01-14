public fun OPCBrowseItem(in _serverHandle, in _strFilter, in _typeFilter)
{
	_ris = 0;
    opcAccess->OPCBrowseItems(  val( _serverHandle ), _strFilter, val( _typeFilter ), xd(_ris));
	return (_ris);
}
