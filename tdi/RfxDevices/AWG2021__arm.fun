public fun AWG2021__arm(as_is _nid, optional _method)
{
	private _N_GPIB_ADDR = 1;

	private _N_TRIGGER_LEVEL = 4;
	private _N_TRIGGER_SLOPE = 5;
	private _N_TRIGGER_IMPEDANCE = 6;
	private _N_TRIGGER_SOURCE = 7;

	private _N_WFM_AMPLITUDE = 9;
	private _N_WFM_OFFSET = 10;
	private _N_WFM_FILTER = 11;
	private _N_WFM_DURATION = 12;
	private _N_WFM_FREQUENCY = 13;
	private _N_WFM_POINTS = 14;



	_nid_head = getnci(_nid, 'nid_number');


	_status = 1;
	_gpib_addr = if_error(data(DevNodeRef(_nid, _N_GPIB_ADDR)), _status = 0);
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on GPIB ADDR');

		abort();
	}


  	_awg2021 = 'GPIB0::' // trim(adjustl(_gpib_addr)) // '::INSTR';


	_handle = 0L;
	_status = AWG2021->Open(_awg2021, ref(_handle));
	write(*, "_status: ", _status);
	_status = AWG2021->Arm(val(_handle));
	write(*, "_status: ", _status);
	_status = AWG2021->Close(val(_handle));
	write(*, "_status: ", _status);

	return(1);
}