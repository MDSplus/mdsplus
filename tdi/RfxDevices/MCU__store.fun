public fun MCU__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 6;
	private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_IP_ADDR = 2;
    private _N_FILTER_POS = 3;
    private _N_MAN_POS = 4;
  
    _ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
	if(_ip_addr == "")
	{
    	DevLogErr(_nid, "Invalid host IP specification");
 	    abort();
	}


	 _man_position = zero(7, 0W);
	 _filter_position = zero(7, 0W);


	_status = TcpClient->MCU_config_values (ref(_ip_addr), ref(_man_position), ref(_filter_position));

	if(_status < 0)
	{
    	DevLogErr(_nid, "Cannot read data from MCU PC");
 	    abort();
	}


write(*, _man_position);
write(*, _filter_position);

	DevPut(_nid, _N_MAN_POS,      _man_position);  
	DevPut(_nid, _N_FILTER_POS,   _filter_position);  

    return(1);
}

