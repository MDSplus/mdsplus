public fun FramesGetInterval(as_is _DevicePath, in _tStart, in _tEnd)
{
	_devType = model_of(_DevicePath);
        _sigOut = 0;
        _status = 0;

	if( _devType == "FLIRSC65X" || _devType == "BASLERACA")
	{	
		 SetTimeContext();
		_path = getnci( _DevicePath , "FULLPATH" );
		_framesPath = build_path(_path//":FRAMES");
		_metaDataPath = build_path(_path//":FRAMES_METAD");
		_framesNid = getnci( _framesPath, "NID_NUMBER");

                if( _tStart >= 0 && _tEnd > _tStart )
		{
		    _status = SetTimeContext( _tStart, _tEnd, *);
                }
                else if ( _tEnd <= 0 && _tStart < _tEnd )
		{
		     _numFrames = 0;
                     write(*, "Before TreeGetNumSegments");
  		     _status = TreeShr->TreeGetNumSegments(val(_framesNid),ref(_numFrames));
                     write(*, "After TreeGetNumSegments");
		     if( _status & 1 )
		     {	
			  _data=0;
			  _dim=0;
                          write(*, "Before TreeGetSegment");
			  _status=TreeShr->TreeGetSegment(val(_framesNid),val(_numFrames-1),xd(_data),xd(_dim));
                          write(*, "After TreeGetSegment");
			  _te = data(_dim)[0];
			  _status = SetTimeContext( ( _te + _tStart ), ( _te + _tEnd ), *);
                     }
		     if( ! (_status & 1) )
		     {
		            write(*, "No frames data");
		     }
                }
                else
                {
                    write(*, "Invalid start end time");
                }

		if( _status & 1 )
		{
			_framesSig = 1;
		        _status=TreeShr->TreeGetRecord(val( _framesNid ), xd( _framesSig ));
			if( _status & 1 )
			{
				_metaDataNid = getnci( _metaDataPath, "NID_NUMBER");
				_metaDataSig = 1;
		    		_status=TreeShr->TreeGetRecord(val( _metaDataNid ),xd( _metaDataSig ));				
			}
			if( _status & 1 )
			{
				_frames = data( _framesSig );
				_dim = dim_of( _framesSig );
				_shape = shape( _frames );
				_metaData = data( _metaDataSig );
				_objectParam = 0;
				_sigOut = make_signal( _frames,, _dim );
			}
		}
	}
	else
	{
		_status = 0;
		write(*, "Device type "//_devType//" not yet supported");
		_sigOut = 0;
	
	}
	SetTimeContext();
	if( !(_status & 1 ) )
		write(*, "Error : "//getmsg(_status));
	return ( _sigOut  );
}
