public fun K3115__reset(as_is _nid, optional _method)
{
	private __CAMAC_NAME = 1;
	private __COMMENT = 2;
	private __CONTROL_MODE = 3;
	private __WAVE_MODE = 4;

	private __CLOCK_SOURCE = 5;
	private __CLOCK_MODE = 6;
	private __CLOCK_FREQ = 7;



    	private __NODES_PER_CHANNEL = 7;
	private __CHANNEL_1 = 8;
	private __RANGE = 1;
	private __RANGE_POL = 2;
	private __TIME_MODE = 3;
	private __OUTPUT = 4;
	private __VOLTAGES = 5;
	private __TIMES = 6;

	private _MASTER = 0;
	private _SLAVE = 1;

	private _CYCLIC = 1;
	private _ACYCLIC = 0;

	private _INTERNAL = 0;
	private _EXTERNAL = 1;

	private _BIPOLAR = 1;
	private _UNIPOLAR = 0;

	private _ASIS = 0;
	private _VARIABLE = 1;

	private _MDS$K_FHUGE = 10E10;



	_camac_name = data(DevNodeRef(_nid, __CAMAC_NAME));

	DevCamChk(_camac_name, CamPiow(_camac_name, 0, 9, _zero = 0, 16), 1, 1);
	DevCamChk(_camac_name, CamPiow(_camac_name, 2, 9, _zero = 0, 16), 1, 1); 
	return(1);
}