public fun L5613__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_MODEL = 3;
    private _N_CHAN_A_OUTPUT = 5;
    private _N_CHAN_A_INPUT  = 6;
    private _N_CHAN_A_RANGE  = 7;
    private _N_CHAN_B_OUTPUT = 9;
    private _N_CHAN_B_INPUT  = 10;
    private _N_CHAN_B_RANGE  = 11;

	_name = DevNodeRef(_nid, _N_NAME);
	DevNodeCvt(_nid, _N_MODEL, ['STD', '100', '200', '300'], [2.,2.5,5.,10.], _constant = 0);
	if(_constant == 0)
	{
		DevLogErr(_nid, "Invalid model specification");
		abort();
	}
	DevNodeCvt(_nid, _N_CHAN_A_RANGE, [100.,50.,20.,10.,5.,2.,1.,0.5,0.2,0.], [0,1,2,4,5,6,8,9,10,12], _range_a=-1);
	if(_range_a == -1)
	{
		DevLogErr(_nid, "Invalid range specification for channel A");
		abort();
	}
	_range_a_path = getnci(DevNodeRef(_nid, _N_CHAN_A_RANGE), 'fullpath');
	_output_a_path = getnci(DevNodeRef(_nid, _N_CHAN_A_OUTPUT), 'fullpath');
	_input_a = compile(_output_a_path // ' * ' //_range_a_path // ' / `_constant ');

	DevNodeCvt(_nid, _N_CHAN_B_RANGE, [100.,50.,20.,10.,5.,2.,1.,0.5,0.2,0.], [0,1,2,4,5,6,8,9,10,12], _range_b=-1);
	if(_range_b == -1)
	{
		DevLogErr(_nid, "Invalid range specification for channel B");
		abort();
	}
	_range_b_path = getnci(DevNodeRef(_nid, _N_CHAN_B_RANGE), 'fullpath');
	_output_b_path = getnci(DevNodeRef(_nid, _N_CHAN_B_OUTPUT), 'fullpath');
	_input_b = compile(_output_b_path // ' * ' //_range_b_path // ' / `_constant ');

	_w = 0;

	DevCamChk(_name, CamPiow(_name, 0, 26, _w, 16),1,*); 
	DevCamChk(_name, CamPiow(_name, 1, 26, _w, 16),1,*); 

	DevCamChk(_name, CamPiow(_name, 0, 16, _range_a, 16),1,*); 
	DevCamChk(_name, CamPiow(_name, 1, 16, _range_b, 16),1,*); 

	DevPut(_nid, _N_CHAN_A_INPUT, _input_a); 
	DevPut(_nid, _N_CHAN_B_INPUT, _input_b); 

	return (1);
}


