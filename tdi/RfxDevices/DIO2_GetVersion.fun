public fun DIO2_GetVersion(in _board_id, optional in _remoteIp)
{
   if( present(_remoteIp) )
   {

	_cmd = 'MdsConnect("'//_remoteIp//'")';
	execute(_cmd);
	_version = MdsValue('DIO2HWGetVersion($1)', _board_id);
	MdsDisconnect();

   }
   else
   {
	_version = DIO2HWGetVersion(_board_id);
   }

   write(*, "DIO2 Firmware version = ", _version);
}
