public fun DevLogErr(in _nid, in _errmsg)
{
    write(*, _errmsg);
	_server = getenv('error_server');
	MdsConnect(_server);
	_actionNid = mdsservershr->GetDoingNid();
	_nids = [_actionNid, _nid];
	MdsValue('Error($1, $2)', _nids, _errmsg);
	MdsDisconnect();
}