public fun MHDReference__load(as_is _nid, optional _method)
{
    private _N_VME_IP = 1;
    private _N_DESCRIPTION = 2;
    private _N_SIG_BASE = 3;
	
	
	private _REFERENCE_SHOT = 100;
	
	_data_valid = 1;
	
	tcl( "set tree mhd_ff/shot = "//_REFERENCE_SHOT );

	_refNid = getnci(build_path("\\MHD_FF::CURR_FF"), "NID_NUMBER");
	_descr = if_error(data(DevNodeRef(_refNid, _N_DESCRIPTION)), _data_valid = 0);
	if(!_data_valid)
	{
			DevLogErr(_nid, 'Invalid description value');
			tcl("close");
			abort();
	}

	tcl("close");

	TreeShr->TreePutRecord( val( DevHead(_nid) + _N_DESCRIPTION  ),  xd( _descr ), val(0) );

    for( _i = 0; _i < 192; _i++ )
    {
	
		tcl( "set tree mhd_ff/shot = "//_REFERENCE_SHOT );

		_refNid = getnci(build_path("\\MHD_FF::CURR_FF"), "NID_NUMBER");

		_y = if_error((data(DevNodeRef(_refNid, _N_SIG_BASE + _i))) , _data_valid = 0);
		_y = data(fs_float(_y));
		if(!_data_valid)
		{
			DevLogErr(_nid, 'Invalid Y value for parameter '// _i);
			tcl("close");
			abort();
		}
		
		_x = if_error(data(dim_of(DevNodeRef(_refNid, _N_SIG_BASE + _i))), _data_valid = 0);
		_x = data(fs_float(_x));
		if(!_data_valid)
		{
			DevLogErr(_nid, 'Invalid X value for parameter '// _i);
			tcl("close");
			abort();
		}
		
		tcl("close");
		
 		_signalNid = DevHead(_nid) + _N_SIG_BASE + _i;

		write(*, getnci( _signalNid , "FULLPATH") );
				
	    _signal = compile('build_signal(`_y,,`_x)');

		TreeShr->TreePutRecord( val( _signalNid ),  xd( _signal ), val(0) );

	}

    return (1);
}
