Public fun OPCReadAsyncGroup(in _serverHandle, in _gName)
{
	_val = 0;
    _status = opcAccess->OPCReadAsyncGroup(  val( _serverHandle ), _gName, xd(_val) );
    return ( _val );
}