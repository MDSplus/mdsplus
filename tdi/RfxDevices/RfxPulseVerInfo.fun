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
	
		if( _urun == 1 )
		{
		    return("Original pulse version\n");
		}
							
		_path = "\\VERSIONS::TOP.URUN_0000"//trim(adjustl(text(_urun)));
		if( _urun > 10 ) _path = "\\VERSIONS::TOP.URUN_000"//trim(adjustl(text(_urun)));
		if( _urun > 100 ) _path = "\\VERSIONS::TOP.URUN_00"//trim(adjustl(_urun));
		if( _urun > 1000 ) _path = "\\VERSIONS::TOP.URUN_0"//trim(adjustl(_urun));
		if( _urun > 10000 ) _path = "\\VERSIONS::TOP.URUN_"//trim(adjustl(_urun));

		
		
		_expList = if_error( data( build_path( _path//":TREES_LIST" )) ,,  []);
		if ( size( _expList ) == 0 )
		{
			return ( "Version not found in pulse" );
		}			
		
		_expVer  = data( build_path( _path//":TREES_VER" ));
		_name    = data( build_path( _path//":NAME" ));
		_reason  = data( build_path( _path//":REASON" ));
		_expReq  = data( build_path( _path//":TREES" ));
		
		_modTreeFlags = zero(size(_expList), 0);
		for( _i = 0; _i < size( _expReq ); _i++ )
		{
		    _modTreeFlags += _expList == _expReq[ _i ];
		}
		_out1 = "Update RUN : "//_urun//"\n";
		/*
		write( *, "URUN   = ", _urun);
		*/
		_out1 = _out1//"\nNAME   = "//_name//"\n";
		/*
		write( *, "REASON = ", _reason);
		*/
		_out1 = _out1//"------------------\n";
		
		for( _i = 0; _i < size( _expList ); _i++ )
		{
		    if( _modTreeFlags[ _i ] )
		    {
			_out1 = _out1//_expList[ _i ]//( _expVer[ _i ]+1 )//" MOD\n";
		    }
		    else
		    {
			_out1 = _out1//_expList[ _i ]//( _expVer[ _i ]+1 )//"\n"; 
		    }
		}
		_out1 = _out1//"------------------\n";

		return( _out1 );
	};
	
	private fun getUrunFromVersion(in _ver_list, in _ver )
	{
	    if( _ver < 0 )
	    {
		_ver = size( _ver_list ) + _ver;
            }

	    if( _ver >= 10000  ) return ( _ver );	
	
	    
	    if( _ver > 1 && _ver <= size( _ver_list ) + 1 )
	    {
		return ( _ver_list[ _ver - 2 ] );
	    }
	    
	    if( _ver == 1 )
	    {
		    return (1); 
	    }
	    
	    if( _ver == 0 )
	    {
		    return ( _ver_list[ size( _ver_list ) - 1 ] ); 
	    }

	    return ( "Version not found in pulse" );
	}
	
	
	private _msg  = "";
	private _ver = [];
	
	
	if( if_error($EXPT,, *) != * )
	{
	
	    if( !present( _exp ) )
                _exp = $EXPT;	    
	
	    if( upcase( _exp ) == "RFX" && $EXPT == "RFX" )
	    {
		
		_msg = "Experiment : "//_exp//"\n";
		    
		_msg = _msg//"Shot       : "//$SHOT//"\n";
		    
		_ver = if_error( data(build_path("\\VERSIONS::URUNS")),,[]);
		    

		if( PRESENT( _version ) )
		{
		     _urun = getUrunFromVersion( _ver, _version );
		     if( kind( _urun ) != 14  )
		     {
			_msg = _msg//printURUNInfo( _urun );
		     }
		     else
		     {
			_msg = _msg//_urun//"\n";
		     }
		}
		else
		{	
			
		     _msg = _msg//"\t   1\t"//RfxShotDate()//"\t    Original\n";
		     
		     for( _i = 0; _i < size( _ver ) ; _i += 1 )
		     {
			_date = if_error(data(build_path("\\VERSIONS::TOP.URUN_"// trim(adjustl( _ver[ _i ] ))//":CREATION_DT")),,"ttt");
			_msg = _msg//(_i+2)//"\t"//_date//" "//_ver[ _i ]//"\n";
		     }
		}
	    }
	    else
	    {
		if( _exp == $EXPT || $EXPT == "RFX" )
		{
			
		   _msg = "Experiment : "//_exp//"\n";
		   
		   _msg = _msg//" Shot       : "//$SHOTNAME//"\n";
			
		   _ver = if_error( data(build_path("\\"//_exp//"::PULSE_VER")),,[]);
		   _k = 1;
		   _msg = _msg//_k//"\t"//RfxShotDate()//"    Original "//"\n";
		   for( _i = 0; _i < size( _ver ) ; _i += 2 )
		   {
			_k++;
			_msg = _msg//_k//"\t"//_ver[ _i ]//"       "//_ver[ _i + 1 ]//"\n";
		   }
		}
		else
		{
		   _msg = _msg//"Pulse file not open\n";
		}
	    }
	}
	else
	{
	    _msg = _msg//"Pulse file not open\n";
	}
	
	return ( _msg );

}