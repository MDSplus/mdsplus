public fun QCS_EVT_WC_NOWAIT(IN _evtwait, OUT _eventout, OUT _shot)
{
	_event = "                    ";
	_valblk = ARRAY(0l,50);
	write (*,_valblk);
	_context = 0Q;
	_evtparam = 0Q;
	_status = evtidl->evt_wc_nowait(_evtwait, DESCR(_event), _valblk,
		 			REF(_evtparam), REF(_context));
	write (*, _status, _event, _valblk);
	_eventout = "THIS IS NOT IT";
	_shot = 0l;
	return(1);
}

