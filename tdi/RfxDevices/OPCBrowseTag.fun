public fun OPCBrowseTag()
{
	_ris = 0;
    MdsOpcClient->OPCBrowseTag(xd(_ris));
	return (_ris);
}
