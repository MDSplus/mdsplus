public fun OPCConnect(optional in _name, in _updateRate)
{
	if(present(_name))
		_status = MdsOpcClient->OPCConnect(_name, val(_updateRate));
	else
		_status = MdsOpcClient->OPCConnect("Softing.OPCToolboxDemo_ServerDA", val(_updateRate));
    return(_status);
}
