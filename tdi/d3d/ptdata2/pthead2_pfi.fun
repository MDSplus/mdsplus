/**************************************************************************************************************************
        PUBLIC FUN PTHEAD2_PFI(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
        
        This TDI function retrieves PFI header info via the IARRAY header

        Input Parameters:  POINTNAME : string - pointname requested from PTDATA 
                           SHOT      : long   - d3d shot number to retrieve

	Known Issues: 
 	 - Not all PFI fields are in the IARRAY
         - If pt_desc has termination character, e.g. fristgrat1, it may be a display problem.
         - The IARRAY header does not zero pad single digit timestamps, e.g. "12:30: 5" which may
           also be a display problem.        

        Note that if one wants to vet the validity of PFI 21 or PFI 42 data then it ought to be
        compared to the displayed results of viwhed which is the authority.

**************************************************************************************************************************/

PUBLIC FUN PTHEAD2_PFI(IN _pointname, OPTIONAL IN _shot) {

        /* Private Function - altrim() strips all whitespaces, via adjustl() and trim() */
        PRIVATE FUN altrim(in _alstring) {
          return(TRIM(ADJUSTL(_alstring)));
        };

        /* Private Function - c() translates int back into a string */
        PRIVATE FUN c(in _iword, in _nchars) {
          _rstring = char(_iword & 0xFF);
          FOR (_i=1; _i<_nchars; _i++) {
            _rstring = _rstring // char((_iword >> _i*8) & 0xFF);
          }
          RETURN(_rstring);
        };

        /* MAIN - PTHEAD_PFI() */
        IF (NOT PRESENT(_shot)) _shot=$SHOT;
        _file = ".PLA";
        _iarray = PTHEAD2_IFIX(_pointname, _shot, 64, _error);
        _pfi = _iarray[4];

        /* PFI 42 Fields */
        if ( EQ(_pfi,42) || EQ(_pfi,41) ) { 
          _exp = c(_iarray[9],4);
          _phase = c(_iarray[10],4);
          _time = altrim(_iarray[14]) // ':' // altrim(_iarray[15]) // ':' // altrim(_iarray[16]);
          _date = altrim(_iarray[17]) // '/' // altrim(_iarray[18]) // '/' // altrim(_iarray[19]);        
          _pt_type = _iarray[20];
          _pt_desc = c(_iarray[21],4) // c(_iarray[22],4) // c(_iarray[23],4) // c(_iarray[24],4);
          _pt_desc = _pt_desc // c(_iarray[25],4) // c(_iarray[26],4);
          _units = c(_iarray[27],4);
          _rev = _iarray[28];
          _zer_off = _iarray[29];
          _dfi = _iarray[30];
        }
        else {
          /* Other PFI */
          RETURN("PFI "//_pfi//" not yet supported.");
        }

        /* viwhed header */
        _vstr = 'POINTNAME: '//_pointname//'\n';
        _vstr = _vstr//'SHOT: '//_shot//'\n';
        _vstr = _vstr//'PFI: '//_pfi//'\n';
        _vstr = _vstr//'EXPERIMENT: '//_exp//'\n';
        _vstr = _vstr//'PHASE: '//_phase//'\n';
        _vstr = _vstr//'TIME(SHOT): '//_time//'\n';
        _vstr = _vstr//'DATE(SHOT): '//_date//'\n';
        _vstr = _vstr//'TYPE: '//_pt_type//'\n';
        _vstr = _vstr//'DESCRIPTION: '//_pt_desc//'\n';
        _vstr = _vstr//'UNITS: '//_units//'\n';
        _vstr = _vstr//'REVISIONS: '//_rev//'\n';
        _vstr = _vstr//'ZERO OFFSET: '//_zer_off//'\n';
        _vstr = _vstr//'DFI: '//_dfi//'\n';
        
        RETURN(_vstr);

}
