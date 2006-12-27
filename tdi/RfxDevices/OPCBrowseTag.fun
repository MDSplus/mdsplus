
public fun OPCBrowseTag(in _filter)
{
	_ris = 0;
	/*
    MdsOpcClient->OPCBrowseTagXd(_filter, xd(_ris));
	*/
    MdsOpcClient->OPCBrowseTag(xd(_ris));
	return (_ris);
}
