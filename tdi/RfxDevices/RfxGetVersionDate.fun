public fun RfxGetVersionDate(in _exp,  in _version )
{

	_status = 1;


	private fun getDate( in _urun )
	{
	
	     _path = "\\VERSIONS::TOP.URUN_0000"//trim(adjustl(text(_urun)))//".CREATION_DT";
	     if( _urun > 10 ) _path = "\\VERSIONS::TOP.URUN_000"//trim(adjustl(text(_urun)))//":CREATION_DT";
	     if( _urun > 100 ) _path = "\\VERSIONS::TOP.URUN_00"//trim(adjustl(_urun))//":CREATION_DT";
	     if( _urun > 1000 ) _path = "\\VERSIONS::TOP.URUN_0"//trim(adjustl(_urun))//":CREATION_DT";
	     if( _urun > 10000 ) _path = "\\VERSIONS::TOP.URUN_"//trim(adjustl(_urun))//":CREATION_DT";
	     
	     _date = if_error( data( build_path( _path ) ), * );

	     return ( _date );
	};
	


    tcl('set view "now"');
    

    if( _version == 0 ) 
	    return ("now");		
	
    if( upcase( _exp ) == "RFX" )
    {
	_uruns = if_error( data(build_path("\\VERSIONS::URUNS")),  _status = 0 );
	
	if( _status & 1 )
	{
	
		if( _version > 9999)
		{
			if( sum( _uruns == _version) )
			{
		
			   _id = sum( (_uruns <= _version) );
			   
			   if( _id == size( _uruns ) )
			     _date = "now";
			   else
			      _date = getDate( _uruns[ _id ] );
			    write(*, "Experiment RFX set pulse file for URUN : "//trim( adjustl(_version) )//" version Date : "//_date);
			    return ( _date );
			}
			else
			    return (0);
		}
	
		
		if( _version < 0 )
		{
		    if( _status & 1 )
		    {
			if( ( size( _uruns ) + _version ) >= 0 )
			{
			    _urun   = _uruns[size( _uruns ) + _version];			 
			    
			    _date = getDate( _urun );
			    
			    if( (size( _uruns ) + _version ) > 0)
				_urun_view = _uruns[size( _uruns ) + _version - 1];
			    else
				_urun_view = " original shot version ";
			}
			else
			    _status = 0;
		    }
		} 
		else
		{
		    if( _status & 1 )
		    {
			_dim = size( _uruns );
			
			_version--;
			    
			if( _version == 0)
			{
				_urun   = _uruns[ _version ];
				_urun_view = " original shot version ";
				_date = getDate( _urun );
			}
			else
			{
				if( _version < _dim )
				{
					_urun   = _uruns[ _version  ];
					_urun_view = _uruns[ _version ];
					_date = getDate( _urun );
				}
				else
				{
					if( _version == _dim )
					{
						_urun_view = _uruns[ _version - 1 ];
						_date      = "NOW";
					}
					else
						_status = 0;
	
					
				}
			}
		     }		    
		}
	}
	else
	{
		if( _version == 1 )
		{
		    /*
		    Pulse file zenza versioni quindi la versione corrente coincide con la versione
		    originale
		    */
		    return ( "now" );
		}
	}
    }
    else
    {
	_ver = if_error( data(build_path("\\"//trim(adjustl(_exp))//"::PULSE_VER")),  _status = 0 );
	
	if( _status & 1)
	{
		if( _version < 0 )
		{
			
		    if( _status & 1 )
		    {
			if( ( size( _ver ) + _version * 2 ) >= 0 )
			{
			    _date   = _ver[size( _ver ) + _version * 2 ];
			    if(( size( _ver ) + _version * 2 ) > 0 )
				_urun_view = _ver[size( _ver ) + _version * 2 - 1];
			    else
				_urun_view = " original shot version ";
			    
			}
			else
			    _status = 0;
		    }
		} 
		else
		{

		    _version--;
			
		    _num_ver = size( _ver )/2;
		    
		    if(  _version <= _num_ver )
		    {
			if( _version == _num_ver )
			    _date   = "now";
			else
			    _date   = _ver[ ( _version ) * 2 ];
			if( _version > 0 )
			{
			    _urun_view = _ver[ _version * 2 - 1];
			}
			else
			{
			    _urun_view = " original shot version ";
			}		    
		    }
		    else
		    {
			_status = 0;
		    }
		}
	}
	else
	{
		if( _version == 1 )
		{
		    /*
		    Pulse file zenza versioni quindi la versione corrente coincide con la versione
		    originale
		    */
		    return ( "now" );
		}
	}
    }	    


    if( ! ( _status & 1 ) )
	return ( 0 );

    if( _exp == "RFX" )
    {    
	write(*, "Experiment RFX set view for URUN ", trim(adjustl(text(_urun_view))) );
    }
    else
    {
	write(*, "Experiment "//_exp//" set view version "//trim(adjustl(text(_version)))//" URUN : "//trim(adjustl(text(_urun_view))) );
    }
    
    write(*, "Set view DATE ", _date);
	

    return ( _date );
}