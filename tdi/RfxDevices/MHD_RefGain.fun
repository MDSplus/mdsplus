public fun MHD_RefGain(in _torPos, in _polPos, optional _control)
{   
    _torPos--;
    _polPos--;

    _chopper =  mod( _torPos,  4) * 2 + _polPos + 1 + 6 * int( _polPos / 2) + 16 * int( _torPos / 4 );

    write(*, _torPos, _polPos, _chopper);
    
    _chopSect = int((_chopper-1)/12);
    _chopId = mod((_chopper-1),12);
    
    _recPathStr = "\\RFX::PR_CONFIG:REC_"//trim(adjustl(text(_chopSect*12+1)))//"_"//trim(adjustl(text((_chopSect+1) * 12)));

    write(*,  _recPathStr, _chopId);

    _recPath = build_path(_recPathStr);

    _values = data( _recPath );
    
    if(_values[12 + _chopId] == 1)
    {
    /*
	write(*, "Chopper "//_chopper//" configurato in anello di corrente");
    */
        if( present(_control) )
        { 
	    if(_control == "I")
	      return (40.0);
	    else
	      return (0.0);
	}
	return (40.0);
    }
    if(_values[24 + _chopId] == 1)
    {/*
	write(*, "Chopper "//_chopper//" configurato in anello di flusso");
     */
        if( present(_control) )
        { 
	    if(_control == "F")
	      return (1./800.);
	    else
	      return (0.0);
	}
	return (1./800.);
    }

    if(_values[36 + _chopId] == 1)
    {
    /*
	write(*, "Chopper "//_chopper//" configurato in anello aperto");
    */
        if( present(_control) )
        { 
	    if(_control == "V")
	      return (69.0);
	    else
	      return (0.0);
	}
	return (69.0);
    }

    return (0);
}
