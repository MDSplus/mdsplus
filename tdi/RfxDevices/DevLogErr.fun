public fun DevLogErr(in _nid, in _errmsg)
{
    write(*, _errmsg);
	if(_nid == 0)
		return(0);

	_server = getenv('error_server');
	MdsConnect(_server);
	_actionNid = MdsServerShr->GetDoingNid();
	MdsValue('Error($1, $2)', _actionNid, _errmsg);
	MdsDisconnect();
}
