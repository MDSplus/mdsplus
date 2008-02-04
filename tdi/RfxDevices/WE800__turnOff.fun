public fun WE800__turnOff(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 35;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_CNTRL_IP = 2;
    private _N_STATION_IP = 3;
    private _N_BUSTRIG_1 = 4;
    private _N_TRIG_SOURCE1 = 5;
    private _N_BUSTRIG_2 = 6;
    private _N_TRIG_SOURCE2 = 7;
    private _N_TRIG_SLOPE = 8;
    private _N_CMN_CLOCK = 9;
    private _N_CLOCK_SOURCE = 10;
    private _N_ARM_SOURCE = 11;

    private _K_NODES_PER_SLOT = 3;
    private _N_SLOT_1= 12;
    private _N_TYPE_MODULE = 1;
    private _N_LINK_MODULE = 2;

write(*,"WE7000 turnOff");

    _controller_ip = if_error(data(DevNodeRef(_nid, _N_CNTRL_IP)), "");
    if(_controller_ip == "")
    {
    	DevLogErr(_nid, "Invalid controller address");
 		abort();
    }

    _station_ip = if_error(data(DevNodeRef(_nid, _N_STATION_IP)), "");
    if(_station_ip == "")
    {
    	DevLogErr(_nid, "Invalid station address");
 		abort();
    }

    _error = we7000->WE7000TurnOffController(_controller_ip);
    if(_error)
    {
	    _msg = repeat(" ", 1024);
		we7000->WE7000GetErrorMsg(ref(_msg));
	    DevLogErr(_nid, _msg);
 		abort();
    }

    return (1);

}
