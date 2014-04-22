public fun OPCServerConnections()
{
   _out = "";
   opcAccess->OpcServerConnections(xd(_out));
   return (_out);
}
