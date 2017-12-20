/*      DENSITY.FUN

	This routine compiles density data from one or more BCI \DEN* signals.

	It will check the \STAT* signals from the BCI tree to determine which times
	are above the threshold and move onto the next chord once it rises above 
	that threshold.  The DEN* / STAT* signals from the BCI tree are used 
	in the following order:
		\DENV2
		\DENR0
		\DENV1
		\DENV3

	Date:			20140926
	Written by:         	Sean Flanagan
	Physicist contact:  	Mike Van Zeeland
*/

PUBLIC FUN DENSITY(OPTIONAL IN _LABEL) {

	/* Note: Ignore the _label.  That's still only there for backward compatability.*/

	PRIVATE FUN GET_DEN(IN _chord, OUT _bci_den, OUT _bci_time, OUT _bci_stat) {
		/* Fetch BCI data for the chord */

		_bden  = USING(DATA(BUILD_PATH("\\DEN"//_chord)), \BCI::TOP, *, "BCI");
                _btime = USING(DATA(DIM_OF(BUILD_PATH("\\DEN"//_chord))), \BCI::TOP, *, "BCI");
                _bstat = USING(DATA(BUILD_PATH("\\STAT"//_chord)), \BCI::TOP, *, "BCI");

		_path  = DATA(BUILD_PATH("\\PATH"//_chord));
		_efit_time = DATA(BUILD_PATH("\\ATIME"));
		
		_mask = ((_btime GE MINVAL(_efit_time))  && (_btime LE MAXVAL(_efit_time)));
		_newtimes = PACK(_btime,_mask);

		/* Interpolate path onto bci time (_newtimes, specifically) */
	   	_mod_path = interpolate_ga( _newtimes[0], _efit_time, _path );
   		for(_i=1;_i<=size(_newtimes)-1;_i++) {
      			_mod_path = [_mod_path,interpolate_ga( _newtimes[_i], _efit_time, _path )];
   		}

		/* BCI DENSITY = DEN / 2. / interpol(PATH) */
		_bci_den  = PACK(_bden,_mask) / 2. / _mod_path;
		_bci_time = _newtimes;
		_bci_stat = PACK(_bstat,_mask); 

        	return(1);
	}

        IF ($SHOT LE 95898) {

                _l = GETNCI(BUILD_PATH("\\DENSV2"),"LENGTH");
                IF (_l GT 0) { _s = BUILD_PATH("\\DENSV2");
                               _label = BUILD_PATH("\\DENSV2:LABEL");
			       _chord = ZERO(SIZE(_s))+4;
			       _stat  = ZERO(SIZE(_s));
                } ELSE { _l = GETNCI(BUILD_PATH("\\DENSR0"),"LENGTH");
                         IF (_l gt 0) { _s = BUILD_PATH("\\DENSR0");
                                        _label = BUILD_PATH("\\DENSR0:LABEL");
					_chord = ZERO(SIZE(_s))+1;
                               		_stat  = ZERO(SIZE(_s));
                         } ELSE { _s = 0.; 
				  _chord = 0.;
			          _stat = 1.;
			 }
                }
		_timebase = DIM_OF(_s);
		PUBLIC __chord = MAKE_SIGNAL( _chord, *, MAKE_WITH_UNITS(_timebase,"ms") );
        	PUBLIC __stat  = MAKE_SIGNAL( _stat,  *, MAKE_WITH_UNITS(_timebase,"ms") );
                RETURN(_s);

        }

	/* Use V2 */
	_status = GET_DEN('V2', _bci_den, _bci_time, _bci_stat);
	_max_time = MAXVAL(_bci_time);
	_stat_threshold = 2.4;

	_stat = ZERO(SIZE(_bci_den));

	_mask = FIRSTLOC(_bci_stat GT _stat_threshold);
	IF (SUM(_mask) EQ 0) { _end_time = _max_time; } ELSE { _end_time = (PACK(_bci_time,_mask))[0]; } 

	_mask = (_bci_time LE _end_time);
	_size = COUNT(_mask);
	_density  = PACK(_bci_den, _mask);
	_timebase = PACK(_bci_time,_mask);
	_chord    = ZERO(_size)+4;
	_stat     = CONDITIONAL(1,_stat,_mask);

	/* Use R0 */
	_stat_threshold = 3.1;
	IF (_end_time LT _max_time) {
		_status = GET_DEN('R0', _bci_den, _bci_time, _bci_stat);

		_beg_time = _end_time;
		_mask = FIRSTLOC( (_bci_stat GT _stat_threshold) && (_bci_time GT _beg_time) );
        	IF (SUM(_mask) EQ 0) { _end_time = _max_time; } ELSE { _end_time = (PACK(_bci_time,_mask))[0]; }

		_mask = ((_bci_time GT _beg_time) && (_bci_time LE _end_time));	
		_size = COUNT(_mask);
        	_density  = [_density, PACK(_bci_den, _mask)];
		_timebase = [_timebase,PACK(_bci_time,_mask)];
		_chord    = [_chord,ZERO(_size)+1];  
		_stat     = CONDITIONAL(1,_stat,_mask);
	}

	/* Use V1 */
 	_stat_threshold = 2.4;
        IF (_end_time LT _max_time) {
		_status = GET_DEN('V1', _bci_den, _bci_time, _bci_stat);

                _beg_time = _end_time;
		_mask = FIRSTLOC( (_bci_stat GT _stat_threshold) && (_bci_time GT _beg_time) );
                IF (SUM(_mask) EQ 0) { _end_time = _max_time; } ELSE { _end_time = (PACK(_bci_time,_mask))[0]; }

		_mask = ((_bci_time GT _beg_time) && (_bci_time LE _end_time));
		_size = COUNT(_mask);
                _density  = [_density, PACK(_bci_den, _mask)];
                _timebase = [_timebase,PACK(_bci_time,_mask)];
		_chord    = [_chord,ZERO(_size)+2];
		_stat     = CONDITIONAL(1,_stat,_mask);
        }

	/* Use V3 */
	_stat_threshold = 3.1;
        IF (_end_time LT _max_time) {
		_status = GET_DEN('V3', _bci_den, _bci_time, _bci_stat);

                _beg_time = _end_time;
		_mask = FIRSTLOC( (_bci_stat GT _stat_threshold) && (_bci_time GT _beg_time) );
                IF (SUM(_mask) EQ 0) { _end_time = _max_time; } ELSE { _end_time = (PACK(_bci_time,_mask))[0]; }

		_mask = ((_bci_time GT _beg_time) && (_bci_time LE _end_time));
		_size = COUNT(_mask);
                _density  = [_density, PACK(_bci_den, _mask)];
                _timebase = [_timebase,PACK(_bci_time,_mask)];
		_chord    = [_chord,ZERO(_size)+8];
		_stat     = CONDITIONAL(1,_stat,_mask);
        }

	PUBLIC __chord = MAKE_SIGNAL( _chord, *, MAKE_WITH_UNITS(_timebase,"ms") );
	PUBLIC __stat  = MAKE_SIGNAL( _stat,  *, MAKE_WITH_UNITS(_timebase,"ms") ); 
	_s = MAKE_SIGNAL( MAKE_WITH_UNITS(_density,"/cm^3"), *, MAKE_WITH_UNITS(_timebase,"ms") );
	return (_s);

}

