public fun DevLogErr(in _nid, in _errmsg)
{
    write(*, _errmsg);

	_server = getenv('error_server');
	if(_server == "") return(0);
	MdsConnect(_server);
	_actionNid = MdsServerShr->GetDoingNid();
	MdsValue('Error($1, $2)', _actionNid, _errmsg);
	MdsDisconnect();
}
