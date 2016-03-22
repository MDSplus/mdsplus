public fun HMSPECTRO__init(as_is _nid, optional _method)
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
    private _OK = 0 ;

    private _HMSPECTRO_SUCCESS = 0;

	_DevicesType = ['C9404MC', 'C9405MC', 'C9404GC', 'C9913GC', 'C9914GB', 'C10082MD', 'C10083MD', 'C9404CA', 'C9404CAH', 'C9405CA', 'C10082CA', 'C10083CA', 'C10083CA', 'C10082CAH', 'C10083CAH' ]; 
	_DevicesCode = [ 0x2905,    0x2905,    0x2905,    0x2907,    0x2907,    0x2908,     0x2908,     0x290D,    0x290D,     0x290D,    0x2909,     0x2909,     0x2909,      0x2909,     0x2909];
	_DevicesPixel= [ 512,       512,       512,       512,       256,       1024,       1024,       1024,      1024,       1024,      2048,       2048,       2048,        2048,       2048];
	_GainNoSwitchDevice = ['C9405MC', 'C9404CA', 'C9404CAH', 'C9405CA', 'C10082CA', 'C10083CA', 'C10083CA', 'C10082CAH', 'C10082CAH' ];

    _dev_name = if_error(data(DevNodeRef(_nid, _N_NAME)), "");
    if( _dev_name == "" )
    {
    	DevLogErr(_nid, "Invalid deice name");
 		abort();
    }

write(*, "HMSPECTRO INIT");
write(*, "_dev_name ", _dev_name);

    DevNodeCvt(_nid, _N_TYPE, _DevicesType, _DevicesCode, _type = _INVALID);
    if( _type == _INVALID )
    {
    	DevLogErr(_nid, "Invalid mini spectrometer device type");
 		abort();
    }

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
	if( _i != size( _DevicesType ) )
		_hwPixel = _DevicesPixel[_i];

write(*, "_hwPixel", _hwPixel);

	_swGain = 1;
    for( _i = 0; _i < size( _GainNoSwitchDevice ) &&  _type_name != _GainNoSwitchDevice[ _i ]; _i++);
	if( _i != size( _GainNoSwitchDevice ) )
		_swGain = 0;

write(*, "_swGain ", _swGain);


    DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EDGE_TRIGGER', 'GATE_TRIGGER'], [0x0,0x1, 0x2], _trig_mode = _INVALID);
    if( _trig_mode == _INVALID )
    {
    	DevLogErr(_nid, "Invalid trigger mode specification");
 		abort();
    }

write(*, "_trig_mode ", _trig_mode);

    DevNodeCvt(_nid, _N_TRIG_EDGE, ['RISING_EDGE', 'FALLING_EDGE', 'NO_FUNCTION'], [0x0,0x1, 0xFF], _trig_edge = _INVALID);
    if( _trig_edge == _INVALID )
    {
    	DevLogErr(_nid, "Invalid trigger edge specification");
 		abort();
    }

write(*, "_trig_edge ", _trig_edge);

    if( _swGain )
	{
        DevNodeCvt(_nid, _N_GAIN, ['LOW', 'HIGH', 'NO_FUNCTION'], [0x0, 0x1, 0xFF], _gain = _INVALID);
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

    _integ_t = if_error(data(DevNodeRef(_nid, _N_INTEG_TIME)), _INVALID);

    if ( size ( _integ_t ) > 1 )
    {
	_integ_t = data( _integ_t );
	_integ_time = _integ_t[ size(_integ_t ) - 1 ];
    }
    else
    {
	_integ_time = _integ_t;
    }

    if( _integ_time == _INVALID )
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
    		DevLogErr(_nid, "Invalid trigger source specification");
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

    _num_s = if_error(data(DevNodeRef(_nid, _N_NUM_SCAN)), _INVALID);

write(*, "_num_scan ", _num_s);

    if ( size (_num_s) > 1 )
    {
	_num_s = data( _num_s );
	_num_scan = _num_s[ size(_num_s ) - 1 ];
    }
    else
    {
	_num_scan = _num_s;
    }

write(*, "_num_scan ", _num_scan);


    if( _num_scan == _INVALID || _num_scan <= 0 )
    {
    	DevLogErr(_nid, "Invalid scan number specification");
 	abort();
    }


write(*, "_num_scan ", _num_scan);

	if(_remote != 0)
	{

		_connected = ( Mdsvalue( "_connected == 1") != * );

		if( ! _connected )
		{
			/*
			* Disconnection is required in case of mdsip server broken connection
			*/
			MdsDisconnect();

			_cmd = 'MdsConnect("'//_ip_addr//'")';
			_status = execute(_cmd);
			if( _status == 0 )
			{
				DevLogErr(_nid,  "Could not open connection to MDS server" );	
				abort();
			}

			_connected = ( MdsValue("public _connected = 1" ) != * );

write(*, " connected ", allocated( _connected ), _connected );

/*
			MdsDisconnect();
			_connected = 0;
*/
		}


		 _status = _HMSPECTRO_SUCCESS;

		if( _trig_mode == 0x2 )
		{
			write(*, " PRE INIZIALIZZAZIONE ");
			_status = MdsValue('HMSPECTRO->HMSpectroInit( $1 , val( $2 ), val( $3 ), val( $4 ), val( $5 ), val( $6 ), val( $7 ) )', 
		                                          _dev_name, _type, _num_scan, _integration_time, _gain, _trig_edge, 0x1);
		}

		
		write(*, " INIZIALIZZAZIONE ", _dev_name, _type, _num_scan, _integration_time, _gain, _trig_edge, _trig_mode );

		if( _status == _HMSPECTRO_SUCCESS )
			_status = MdsValue('HMSPECTRO->HMSpectroInit( $ , val( $ ), val( $ ), val( $ ), val( $ ), val( $ ), val( $ ) )', 
		                                          _dev_name, _type, _num_scan, _integration_time, _gain, _trig_edge, _trig_mode);
		
		if( _status != _HMSPECTRO_SUCCESS )
		{
			_msg = MdsValue('HMSPECTROGetMsg( $1 )', _status );
		}
/*
		MdsDisconnect();
*/
	}
	else
	{

		_status = HMSPECTRO->HMSpectroInit( _dev_name , 
											val( _type ), 
											val( _num_scan ), 
											val( _integration_time ), 
											val( _gain ), 
											val( _trig_edge ), 
											val( _trig_mode ) );

		if( _status != _HMSPECTRO_SUCCESS )
		{
			_msg = repeat(' ', 200);
			HMSPECTRO->HMSpectroGetMsg( val(_status), ref( _msg ) );
		}

	}

	if( _status != _HMSPECTRO_SUCCESS )
	{
    	DevLogErr(_nid, _msg );	
		abort();
	}
	

    return ( 1 );
}
