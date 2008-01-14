Public fun OPCReadGroup(in _serverHandle, in _gName)
{
	_val = 0;
    _status = opcAccess->OPCReadGroup(  val( _serverHandle ), _gName, xd(_val) );
    return ( _val );
}