public fun OPCDisableAsyncReadGroup(in _serverHandle, in _gName)
{
    _status = opcAccess->OPCDisableAsyncReadGroup(  val( _serverHandle ), _gName);
     return(_status);
}
