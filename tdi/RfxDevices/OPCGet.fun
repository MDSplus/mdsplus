public fun OPCGet(in _tag)
{
    _ris = 0;
    _status = MdsOpcClient->OPCRead(_tag, xd(_ris));
     return(_ris);
}
