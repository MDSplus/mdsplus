public fun TR10_Config(in _board_id, optional in _remoteIp)
{
   if( present(_remoteIp) )
	{
		_cmd = 'MdsConnect("'//_remoteIp//'")';
		execute(_cmd);
		_status = MdsValue('TR10HWConfig($1)', _board_id);
		MdsDisconnect();
		if(_status == 0)
			{
				DevLogErr(_nid, "Error Configuration TR10 device: seet CPCI console for details");
				abort();
			}
	}	
   else
	{
		_status = TR10HWConfig(_board_id);
	}

   write(*, "TR10 Configured = ", _status);
}
