public fun OPCConnect()
{
    write(*, "OPC Connect");
    _status = MdsOpcClient->OPCConnect("Softing.OPCToolboxDemo_ServerDA");
    return(_status);
}
