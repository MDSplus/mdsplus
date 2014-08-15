public fun ACQD265__trigger(as_is _nid, optional _method)
{
    private _N_MAME = 1;
    private _N_COMMENT = 2;
    private _N_SERIAL_NUM = 3;
    private _N_SLOT_NUM = 4;
    private _N_CLOCK_MODE = 5;
    private _N_CLOCK_SOURCE = 6;
    private _N_TEMPERATURE = 7;
    private _N_FREQUENCY = 8;

    private _N_SEGM_POINTS = 10;
    private _N_SEGM_NUMBER = 11;

    private _N_TRIG_MODE = 13;
    private _N_TRIG_SOURCE = 14;
    private _N_TRIG_DELAY = 15;
    private _N_TRIG_CHAN = 16;
    private _N_TRIG_COUPLING = 17;
    private _N_TRIG_SLOPE = 18;
    private _N_TRIG_LEVEL = 19;

	private _K_NODES_PER_CHANNEL = 6;

	private _N_CHANNEL_0= 20;
	private _N_CHAN_FULL_SCALE = 1;
	private _N_CHAN_OFFSET = 2;
	private _N_CHAN_COUPLING = 3;	
	private _N_CHAN_BANDWIDTH = 4;


write(*, 'Parte ACQD265__trigger');

    _name = if_error(data(DevNodeRef(_nid, _N_MAME)),(DevLogErr(_nid, "Missing Instrument name"); return (1);));

write(*, 'Name: ', _name);

	_status = ACQD265->trigger(_name);

	if( _status )
	{
		DevLogErr(_nid, ACQD265_ErrorToString(_status)); 
		Abort();
	}

    return (1);
}