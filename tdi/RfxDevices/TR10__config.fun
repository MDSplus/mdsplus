public fun TR10__config(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 45;
    private _N_HEAD = 0;
    private _N_BOARD_ID = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;
    private _N_COMMENT = 4;
    private _N_CLOCK_MODE = 5;
    private _N_TRIG_MODE = 6;
    private _N_TRIG_SOURCE = 7;
    private _N_CLOCK_SOURCE = 8;
    private _N_FREQUENCY = 9;
    private _N_USE_TIME = 10;
    private _N_PTS = 11;
    private _K_NODES_PER_CHANNEL = 6;
    private _N_CHANNEL_0= 12;
    private _N_CHAN_START_TIME = 1;
    private _N_CHAN_END_TIME = 2;
    private _N_CHAN_START_IDX = 3;
    private _N_CHAN_END_IDX = 4;
    private _N_CHAN_DATA = 5;
    private _N_TRIG_EDGE = 108;
    private _N_INIT_ACTION = 109;
    private _N_STORE_ACTION = 110;
    private _2M = 2097152;


   private _INVALID = 10E20;

    write(*, 'TR10 Config');

    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
        DevLogErr(_nid, "Invalid Board ID specification");
                abort();
    }
  else
	{
		_version = TR10HWGetVersion(_board_id);
		_status = TR10HWConfig(_board_id);
	}

   write(*, "TR10 Firmware version = ", _version);
   write(*, "TR10 Configured =", _status);
   return(1);
}
