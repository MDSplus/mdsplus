public fun QCS_EVT_SUBSCRIBE(IN _evt)
{
	_param = *;
	evtidl->evt_subscribe(_evt, _param);
	return(1);
}
