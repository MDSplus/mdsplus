public fun TR32__config(as_is _nid, optional _method)
{

	private _N_HEAD = 0;
	private _N_BOARD_ID = 1;
	private _N_SW_MODE = 2;
	private _N_IP_ADDR = 3;
	private _N_COMMENT = 4;
	private _N_CLOCK_MODE = 5;
	private _N_TRIG_MODE = 6;
	private _N_TRIG_SOURCE = 7;
	private _N_CLOCK_SOURCE = 8;
	private _N_INT_FREQUENCY = 9;
	private _N_CK_RESAMPLING = 10;
	private _N_CK_TERMINATION = 11;
	private _N_USE_TIME = 12;
	private _N_PTS = 13;
	private _N_TRIG_EDGE = 14;
	private _N_CHANNEL_0= 15;

	private _K_NODES_PER_CHANNEL = 7;
	private _N_CHAN_RANGE = 1;
	private _N_CHAN_START_TIME = 2;
	private _N_CHAN_END_TIME = 3;
	private _N_CHAN_START_IDX = 4;
	private _N_CHAN_END_IDX = 5;
	private _N_CHAN_DATA = 6;

	private _64M = 67108864;
	private _INVALID = 10E20;




    write(*, 'TR32 Config');

    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
        DevLogErr(_nid, "Invalid Board ID specification");
                abort();
    }
  else
	{
		_version = TR32HWGetVersion(_board_id);
		_status = TR32HWConfig(_board_id);
	}

   write(*, "TR32 Firmware version = ", _version);
   write(*, "TR32 Configured =", _status);
   return(1);
}
