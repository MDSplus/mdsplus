public fun RfxPulseVerInfo(optional in _exp, optional in _version)
{
    
	private fun getUrunPath( in _urun )
	{
	
	     _path = "\\VERSIONS::TOP.URUN_0000"//trim(adjustl(text(_urun)));
	     if( _urun > 10 ) _path = "\\VERSIONS::TOP.URUN_000"//trim(adjustl(text(_urun)));
	     if( _urun > 100 ) _path = "\\VERSIONS::TOP.URUN_00"//trim(adjustl(_urun));
	     if( _urun > 1000 ) _path = "\\VERSIONS::TOP.URUN_0"//trim(adjustl(_urun));
	     if( _urun > 10000 ) _path = "\\VERSIONS::TOP.URUN_"//trim(adjustl(_urun));
	     
	     return ( _path );
	};

	private fun printURUNInfo( in _urun )
	{
		_out = "";
	
		if( _urun == 0 )
		{
		    _out = "Original pulse version\n";
		    return(0);
		}
							
		_path = "\\VERSIONS::TOP.URUN_0000"//trim(adjustl(text(_urun)));
		if( _urun > 10 ) _path = "\\VERSIONS::TOP.URUN_000"//trim(adjustl(text(_urun)));
		if( _urun > 100 ) _path = "\\VERSIONS::TOP.URUN_00"//trim(adjustl(_urun));
		if( _urun > 1000 ) _path = "\\VERSIONS::TOP.URUN_0"//trim(adjustl(_urun));
		if( _urun > 10000 ) _path = "\\VERSIONS::TOP.URUN_"//trim(adjustl(_urun));

		_expList = data( build_path( _path//":TREES_LIST" ));		
		_expVer  = data( build_path( _path//":TREES_VER" ));
		_name    = data( build_path( _path//":NAME" ));
		_reason  = data( build_path( _path//":REASON" ));
		_expReq  = data( build_path( _path//":TREES" ));
		
		_modTreeFlags = zero(size(_expList), 0);
		for( _i = 0; _i < size( _expReq ); _i++ )
		{
		    _modTreeFlags += _expList == _expReq[ _i ];
		}
		/*
		write( *, "URUN   = ", _urun);
		*/
		_out = _out//"NAME   = "//_name//"\n";
		/*
		write( *, "REASON = ", _reason);
		*/
		_out = _out//"------------------\n";
		
		for( _i = 0; _i < size( _expList ); _i++ )
		{
		    if( _modTreeFlags[ _i ] )
		    {
			_out = _out//_expList[ _i ]//( _expVer[ _i ]+1 )//" MOD\n";
		    }
		    else
		    {
			_out = _out//_expList[ _i ]//( _expVer[ _i ]+1 )//"\n"; 
		    }
		}
		_out = _out//"------------------\n";

		return( _out );
	};
	
	private fun getUrunFromVersion(in _ver_list, in _ver )
	{
	    if( _ver < 0 )
	    {
		_ver = size( _ver_list ) + _ver;
            }

	    if( _ver >= 10000 || _ver == 0 ) return ( _ver );	
	
	    
	    if( _ver > 0 && _ver <= size( _ver_list ) )
	    {
		return ( _ver_list[ _ver - 1 ] );
	    }

	    return ( "Version not found in pulse" );
	}
	
	_out="";
	
	if( if_error($EXPT,, *) != * )
	{
	
	    if( !present( _exp ) )
                _exp = $EXPT;	    
	
	    if( upcase( _exp ) == "RFX" && $EXPT == "RFX" )
	    {
		
		_out = "Experiment : "//_exp//"\n";
		    
		_out = _out//"Shot       : "//$SHOT//"\n";
		    
		_ver = if_error( data(build_path("\\VERSIONS::URUNS")),,[]);

		if( PRESENT( _version ) )
		{
		     _urun = getUrunFromVersion( _ver, _version );
		     if( kind( _urun ) != 14  )
			_out = _out//printURUNInfo( _urun );
		     else
		     {
			_out = _out//_urun//"\n";
		     }
		}
		else
		{
		
		     _out = _out//"\tOriginal\n";
		     
		     for( _i = 0; _i < size( _ver ) ; _i += 1 )
		     {
			_out = _out//(_i+2)//_ver[ _i ]//"\n";
		     }
		}
	    }
	    else
	    {
		if( _exp == $EXPT || $EXPT == "RFX" )
		{
			
		   _out = "Experiment : "//_exp//"\n";
		   
		   _out = _out//"Shot       : "//$SHOTNAME//"\n";
			
		   _ver = if_error( data(build_path("\\"//_exp//"::PULSE_VER")),,[]);
		   _k = 1;
		   _out = _out//_k//"    "// "Original"//"\n";
		   for( _i = 0; _i < size( _ver ) ; _i += 2 )
		   {
			_k++;
			_out = _out//_k//"   "//_ver[ _i ]//"   "//_ver[ _i + 1 ]//"\n";
		   }
		}
		else
		{
		   _out = _out//"Pulse file not open\n";
		}
	    }
	}
	else
	{
	    _out = _out//"Pulse file not open\n";
	}
	
	return ( _out );

}