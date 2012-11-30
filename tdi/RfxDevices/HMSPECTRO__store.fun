public fun HMSPECTRO__store(as_is _nid, optional _method)
{

	private _connected = 0;


    private _K_CONG_NODES = 16;
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;
    private _N_COMMENT = 4;
    private _N_TYPE = 5;
    private _N_TRIG_MODE = 6;
    private _N_TRIG_EDGE = 7;
    private _N_TRIG_SOURCE = 8;
    private _N_NUM_SCAN = 9;
    private _N_GAIN = 10;
    private _N_INTEG_TIME = 11;
    private _N_CALIBRATION = 12;
    private _N_DATA = 13;
 
 
    private _INVALID = -1 ;
    private _OK = 1 ;
	
    private _HMSPECTRO_SUCCESS = 0;
	private _HMSPECTRO_INVALID_BUFFER_ALLOCATION = 9004;
    private _HMSPECTRO_NOT_ACQUIRED_ALL_FRAME = 9005;


	_DevicesType = ['C9404MC', 'C9405MC', 'C9404GC', 'C9913GC', 'C9914GB', 'C10082MD', 'C10083MD', 'C9404CA', 'C9404CAH', 'C9405CA', 'C10082CA', 'C10083CA', 'C10083CA', 'C10082CAH', 'C10083CAH' ]; 
	_DevicesCode = [ 0x2905,    0x2905,    0x2905,    0x2907,    0x2907,    0x2908,     0x2908,     0x290D,    0x290D,     0x290D,    0x2909,     0x2909,     0x2909,      0x2909,     0x2909];
	_DevicesPixel= [ 512,       512,       512,       512,       256,       1024,       1024,       1024,      1024,       1024,      2048,       2048,       2048,        2048,       2048];
	_GainSwitchDevice = ['C9405MC', 'C9404CA', 'C9404CAH', 'C9405CA', 'C10082CA', 'C10083CA', 'C10083CA', 'C10082CAH', 'C10082CAH' ];

    _dev_name = if_error(data(DevNodeRef(_nid, _N_NAME)), "");
    if( _dev_name == "" )
    {
    	DevLogErr(_nid, "Invalid deice name");
 		abort();
    }

write(*, "_dev_name ", _dev_name);

    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);
	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    DevLogErr(_nid, "Invalid Crate IP specification");
 		    abort();
		}
	}


    DevNodeCvt(_nid, _N_TYPE, _DevicesType, _DevicesCode, _type = _INVALID);
    if( _type == _INVALID )
    {
    	DevLogErr(_nid, "Invalid mini spectrometer device type");
 		abort();
    }

write(*, "_type code ", _type);

    _type_name = if_error(data(DevNodeRef(_nid, _N_TYPE)), "");
    if( _dev_name == "" )
    {
    	DevLogErr(_nid, "Invalid deice name");
 		abort();
    }

write(*, "_type name ", _type_name);


	_hwPixel = 0;
    for( _i = 0; _i < size( _DevicesType ) && _type_name != _DevicesType[ _i ]; _i++);
	if( _i != size( _GainSwitchDevice ) )
		_hwPixel = _DevicesPixel[_i];

write(*, "_hwPixel", _hwPixel);

	_swGain = 0;
    for( _i = 0; _i < size( _GainSwitchDevice ) &&  _type_name != _GainSwitchDevice[ _i ]; _i++);
	if( _i != size( _GainSwitchDevice ) )
		_swGain = 1;

write(*, "_swGain ", _swGain);

    DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EDGE_TRIGGER', 'GATE_TRIGGER'], [0x0, 0x1, 0x2], _trig_mode = _INVALID);
    if( _trig_mode == _INVALID )
    {
    	DevLogErr(_nid, "Invalid trigger mode specification");
 		abort();
    }

write(*, "_trig_mode ", _trig_mode);

    DevNodeCvt(_nid, _N_TRIG_EDGE, ['RISING_EDGE', 'FALLING_EDGE', 'NO_FUNCTION'], [0x0, 0x1, 0xFF], _trig_edge = _INVALID);
    if( _trig_edge == _INVALID )
    {
    	DevLogErr(_nid, "Invalid trigger edge specification");
 		abort();
    }

write(*, "_trig_edge ", _trig_edge);

    if( _swGain )
	{
        DevNodeCvt(_nid, _N_GAIN, ['LOW', 'HIGH', 'NO_FUNCTION'], [0x0,0x1, 0xFF], _gain = _INVALID);
        if( _gain == _INVALID )
        {
    	    DevLogErr(_nid, "Invalid gain specification");
 		    abort();
        }
	}
	else
	{
		_gain = 0xFF;
		DevPut(_nid, _N_GAIN, 'NO_FUNCTION');
	}

write(*, "_gain ", _gain);


    _integ_time = if_error(data(DevNodeRef(_nid, _N_INTEG_TIME)), _INVALID);
    if( _integ_time == _INVALID)
    {
    	DevLogErr(_nid, "Invalid integration time specification");
 		abort();
    }
    _integration_time = LONG( _integ_time * 1000000 );

write(*, "_integration_time ", _integration_time);


	if( _trig_mode != 0x0)
	{

		_status = _OK;
/*
		_clock = if_error( evaluate(DevNodeRef(_nid, _N_TRIG_SOURCE)), _status = _INVALID);
		if( _status == _INVALID )
		{
    		DevLogErr(_nid, "Invalid scan number specification");
 			abort();
		}
		_clock = execute('evaluate(`_clock)');
		_trig_time = dscptr(_clock, 0);
		_delta = dscptr(_clock, 2);
*/
		
		_trig_time = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _status = _INVALID);
		if( _status == _INVALID )
		{
    		DevLogErr(_nid, "Invalid scan number specification");
 			abort();
		}
		_delta = _integ_time;

	}
	else
	{
		_trig_time = 0;
		_delta = _integ_time;
	}

write(*, "_trig_time ", _trig_time);

    _num_scan = if_error(data(DevNodeRef(_nid, _N_NUM_SCAN)), _INVALID);
    if( _num_scan == _INVALID || _num_scan <= 0)
    {
    	DevLogErr(_nid, "Invalid scan number specification");
 		abort();
    }
	
write(*, "_num_scan ", _num_scan);

	_bufSize = _hwPixel * _num_scan;

write(*, "_bufSize ", _bufSize);

	if(_remote != 0)
	{
		_connected = if_error(Mdsvalue( "_connected" ) == 1, 1, 0);

		if( _connected == 0  )
		{
			DevLogErr(_nid,  "Connection to MDS server is not established by the init operation" );	
			abort();
		}

/*

		_cmd = 'MdsConnect("'//_ip_addr//'")';
		_status = execute(_cmd);
		if( _status == 0 )
		{
			DevLogErr(_nid,  "Could not open connection to MDS server" );	
			abort();
		}
*/
	    _lambda = MdsValue('HMSPECTROReadLambda( $1,  $2 )',  _dev_name ,  _hwPixel);

		_status = _HMSPECTRO_SUCCESS;
		if(size( _lambda ) == 1)
			_status = _lambda[0];

		if( _status != _HMSPECTRO_SUCCESS )
		{
			_msg = MdsValue('HMSPECTROGetMsg( $1 )', _status );
		}
		else
		{

			_data = MdsValue('HMSPECTROReadData( $1,  $2 )',  _dev_name ,  _bufSize);
	
			_status = _HMSPECTRO_SUCCESS;
			if(size( _data ) == 1)
				_status = _data[0];
	
			if( _status != _HMSPECTRO_SUCCESS )
			{
				_msg = MdsValue('HMSPECTROGetMsg( $1 )', _status );
			}
			else
			{
				_tbase = MdsValue('HMSPECTROReadTbase( $1,  $2 )',  _dev_name ,  _num_scan);
				write(*, "Tbase ",  _tbase  );
			}
		}

		MdsValue( 'HMSPECTRO->HMSpectroClose($1)', _dev_name );

/*
		MdsDisconnect();
*/
	}
	else
	{

		_data = zero( _bufSize, 0WU); 
		_tbase = zero( _num_scan, fs_float(0.0));
		_lambda = zero( _pixel, ft_float(0.0) ); 


		_status = HMSPECTRO->HMSpectroReadLambda( _dev_name, ref( _lambda ), val( _hwPixel ));

		if( _status != _HMSPECTRO_SUCCESS )
		{
			_msg = repeat(' ', 200);
			HMSPECTRO->HMSpectroGetMsg( val( _status ), ref( _msg ) );
		}


		_status = HMSPECTRO->HMSpectroReadData( _dev_name, ref( _data ), val( _bufSize ));

		if( _status != _HMSPECTRO_SUCCESS )
		{
			_msg = repeat(' ', 200);
			HMSPECTRO->HMSpectroGetMsg( val( _status ), ref( _msg ) );
		} else {

			HMSPECTRO->HMSpectroReadTbase( _dev_name, ref( _tbase ), val( _num_scan ));
			write(*, "Tbase ", _tbase);
		}
		HMSPECTRO->HMSpectroClose( _dev_name );


	}

	if( _status != _HMSPECTRO_SUCCESS )
	{
		if( _status !=	_HMSPECTRO_INVALID_BUFFER_ALLOCATION && _status != _HMSPECTRO_NOT_ACQUIRED_ALL_FRAME )
		{
    		DevLogErr( _nid, _msg );
 			abort();
		}
		else
		{
			write(*, _msg);
		}
	}


	_t0 = _trig_time + _integ_time/2;
	
	_tbase = accumulate( _tbase ) + _t0;

	_buf = transpose( set_range(_hwPixel, _num_scan, _data) );

/*
    _dim1 = make_dim(make_window(0, _num_scan - 1, _t0 ),  make_range(*,*,_integ_time));
*/
    _dim1 = make_dim(make_window(0, _num_scan - 1, _t0 ),  _tbase);

    _dim2 = make_with_units(( _lambda ),"nm");

	_data_nid = DevHead(_nid) + _N_DATA;
    _signal = compile('build_signal((`_buf), $VALUE, (`_dim1), (`_dim2))');

    return (TreeShr->TreePutRecord(val(_data_nid),xd(_signal),val(0)));

}
