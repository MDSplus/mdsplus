public fun DevLogErr(in _nid, in _errmsg)
{
 	public _last_device_error = _errmsg;
    	write(*, _errmsg);
	_dispatched = 1;
	_action_nid = if_error(_doing_nid, _dispatched = 0);
	
	if(!_dispatched)
	    return(1);

	_server = getenv('error_server');
	if(_server == "") return(0);
	MdsConnect(_server);
	/*_actionNid = MdsServerShr->GetDoingNid();*/
	MdsValue('Error($1, $2)', _action_nid, _errmsg);
	MdsDisconnect();
}
