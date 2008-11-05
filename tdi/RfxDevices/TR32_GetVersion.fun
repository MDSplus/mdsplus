public fun TR32_GetVersion(in _board_id, optional in _remoteIp)
{
   if( present(_remoteIp) )
   {

	_cmd = 'MdsConnect("'//_remoteIp//'")';
	execute(_cmd);
	_version = MdsValue('TR32HWGetVersion($1)', _board_id);
	MdsDisconnect();

   }
   else
   {
	_version = TR32HWGetVersion(_board_id);
   }

   write(*, "TR32 Firmware version = ", _version);
}
