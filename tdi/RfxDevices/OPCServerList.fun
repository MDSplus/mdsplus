public fun OPCServerList(optional in _remoteServer)
{
   _out = "";
   if (present(_remoteServer))
		MdsOpcClient->OPCServerList(xd(_out), _remoteServer);
   else
		MdsOpcClient->OPCServerList(xd(_out), val(0));

   return (_out);
}
