/*	MPB__DECODER_DCLOCK.FUN
	Computes clock range for MPB decoder dual speed clock.
        _tstart is the time the scalers started.
        _dt_1 is 1/the first frequency in hz.
        _dt_2 is 1/the second frequency in hz.
        _start_low specificies whether the clock starts out low.
        _gate is signal used to gate the decoder channel.
        _rising denotes return range representing the rising edges.

	Tom Fredian, MIT PFC  1993


    The following describes the behavior of the MPB dual speed
    clock. The MIT_DCLOCK device always starts the clock with
    output low.

    The clock always finishes the current hi or low state before it
    switches to the next frequency. If the gate occurs exactly on a
    state boundary it completes the next state before switching. If
    the length of the hi or low state is less than 6 microseconds
    the clock will output one additional hi or low state before
    switching frequency.
     
     ================ dt/2 > 5 =======================
      ___ ___ ___ ___ _____   _____   _____   _____   
      | |_| |_| |_| |_|   |___|   |___|   |___|   |___
      r f r f r f r f r   f   r   f   r   f   r   f
                    ||
                    G
     
      timestamps for rising edges:
      *   *   *   *   *       *       *       *
                      E       S
      timestamps for falling edges:
        *   *   *   *     *       *       *       *
                    E     S

     ================ dt/2 > 5 =======================
      ___ ___ ___ ___    _____   _____   _____   _____
      | |_| |_| |_| |____|   |___|   |___|   |___|
      r f r f r f r f    r   f   r   f   r   f   r
                  ||
                  G

      timestamps for rising edges:
      *   *   *   *      *       *       *       *
                  E      S
      timestamps for falling edges:
        *   *   *   *        *       *       *       
                    E        S

     ================ dt/2 < 6 =======================
      ___ ___ ___ ___ _____   _____   _____   _____   
      | |_| |_| |_| |_|   |___|   |___|   |___|   |___
      r f r f r f r f r   f   r   f   r   f   r   f
                  ||
                  G

      timestamps for rising edges:
      *   *   *   *   *       *       *       *
                      E       S
      timestamps for falling edges:
        *   *   *   *     *       *       *       *
                    E     S

     ================ dt/2 < 6 =======================
      ___ ___ ___ ___    _____   _____   _____   _____
      | |_| |_| |_| |____|   |___|   |___|   |___|
      r f r f r f r f    r   f   r   f   r   f   r
                ||
                G

      timestamps for rising edges:
      *   *   *   *      *       *       *       *
                  E      S
      timestamps for falling edges:
        *   *   *   *        *       *       *       
                    E        S
*/

FUN PUBLIC MPB__DECODER_DCLOCK(in _tstart, in _dt_1, in _dt_2, in _start_low, in _gate, in _rising) {

  _tstart = f_float(_tstart);
  _frequency_1 = f_float(1./_dt_1);
  _frequency_2 = f_float(1./_dt_2);
  _max_period = 1/minval([_frequency_1,_frequency_2]);
  for (_source = 1, _period = 1E-6;
       _period * 65534 < _max_period && _source <= 5; _source++, _period *= 10);
  _load = nint(1/_frequency_1/_period/2);
  _hold = nint(1/_frequency_2/_period/2);
  if (_load == 0) { _load++;}
  if (_hold == 0) { _hold++;}
  _dt = [_period * _load * 2,_period * _hold * 2];

  /*********************************************
  *  Convert all times to nearest microsecond  *
  *********************************************/

  _falling = !_rising;
  _tstart = NINT(_tstart*1E6);
  _gate = NINT(_gate*1E6);
  _dt = NINT(_dt * 1E6);

  /****************************************
  *  Compute number of frequency changes  *
  *  and make vector of clock dt's        *
  *  at 1/2 the width of a full cycle.    *
  ****************************************/

  _n = size(_gate);
  _dts = replicate(_dt/2,0,_n/2 + 1)[0:(_n)];
  
  /**************************************************
  *  Since clock starts out with output low         *
  *  the first rising edge occurs when the scalers  *
  *  start plus 1/2 a cycle. The first falling      *
  *  edge occurs one full cycle after the scalers   *
  *  start.                                         *
  **************************************************/

  _starts = _start = _tstart + _dts[0] * (((_falling && _start_low) || !(_falling || _start_low))? 2 : 1);
  _ends = *;

  /**************************************************
  *  For each frequency shift, compute the end of   *
  *  the current range and the beginning of the     *
  *  next range.                                    *
  **************************************************/

  for (_i=0;_i<_n;_i++) {

    /****************************************************
    *  Compute the total number of hi/lo state changes  *
    *  the clock will complete since the beginning of   *
    *  this frequency range to the beginning of the     *
    *  next frequency range.                            *
    ****************************************************/

    _nt = (_gate[_i] - _start)/_dts[_i] + (_dts[_i] > 5 ? 1 : 2);

    /****************************************************
    *  Compute the end of this frequency range and the  *
    *  start of the next frequency range.               *
    ****************************************************/

    _end = _start + _nt * _dts[_i] - ((_nt & 1) ? _dts[_i] : 0);
    _start = _end + ((_nt & 1) ? _dts[_i] + _dts[_i+1] : 2 * _dts[_i+1]);

    _ends = [_ends,_end];
    _starts = [_starts,_start];
  }

  /******************************************
  *  Convert all times to back to seconds.  *
  ******************************************/

  _starts = _starts*1E-6 - 1E-8;
  _ends = [_ends*1E-6,$ROPRAND];
  _dts = _dts * 2e-6;

  return(make_range(_starts, _ends,_dts));
}
