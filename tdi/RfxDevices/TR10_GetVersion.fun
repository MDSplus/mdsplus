public fun TR10_GetVersion(in _board_id, optional in _remoteIp)
{
   if( present(_remoteIp) )
   {

	_cmd = 'MdsConnect("'//_remoteIp//'")';
	execute(_cmd);
	_version = MdsValue('TR10HWGetVersion($1)', _board_id);
	MdsDisconnect();

   }
   else
   {
	_version = TR10HWGetVersion(_board_id);
   }

   write(*, "TR10 Firmware version = ", _version);
}
