public fun OPCGet(in _tag)
{
    _ris = 0;
    _status = 0;
    _status = MdsOpcClient->OPCRead(_tag, xd(_ris));
	 write(*, "In ", _ris, _status);
     return(_ris);
}
