public fun DevLogErr(in _nid, in _errmsg)
{
    write(*, _errmsg);
	_server = getenv('error_server');
	MdsConnect(_server);
	_actionNid = mdsservershr->GetDoingNid();
	MdsValue('Error($1, $2)', _actionNid, _errmsg);
	MdsDisconnect();
}