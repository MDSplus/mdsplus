public fun dt200da__store(as_is _nid, optional _method)
{
  _DTYPE_RANGE = 201BU;
   _DT200_NODE = 1;
   _DT200_BOARD = 2;
   _DT200_DIs = 4;
   _DT200_NODES_PER_DI = 3;
   _DT200_DI_WIRE=1;
   _DT200_DI_BUS=2;
   _DT200_CLOCK_SRC=22;
   _DT200_CLOCK_DIV=23;
   _DT200_DAQ_MEM=24;
   _DT200_ACTIVE_CHAN=25;
   _DT200_TRIG_SRC=26;
   _DT200_POST_TRIG=27;
   _DT200_PRE_TRIG=28;
   _DT200_AI_CHANS = 29;
   _DT200_NODES_PER_AI = 5;
   _DT200_AI_START = 1;
   _DT200_AI_END = 2;
   _DT200_AI_INC = 3;
   _DT200_AI_COEFFS = 4;

   _DI_NUMBERS = BUILD_SIGNAL([0,1,2,3,4,5], *, ['DI0', 'DI1', 'DI2', 'DI3', 'DI4', 'DI5' ]);

  _node = if_error(data(DevNodeRef(_nid,_DT200_NODE)), "");
  if (Len(_node) <= 0) {
     _node = 'local';
  }
  _status = MdsConnect(_node);
  if ((_status < 0) && (_node != 'local')) {
    Write(*,"Could not connect to "//_node);
    Abort();
  }
  _board = if_error(data(DevNodeRef(_nid,_DT200_BOARD)), DevError("Dt200 board must be specified"));

  /***********************************
   See if digitizer is done digitizing
  ************************************/

  _state = MdsValue('Dt200WriteMaster($, "getState")', _board);
  if (_state != "ACQ32:0 ST_STOP") {
    write(*, '%DT200ERR, board '//_board//' Device not triggered');
    Abort();
  }

/* should be able to ask the board getClock AI and then look it up but
   for now assume the board did what we asked */

  _clockSource = if_error(data(DevNodeRef(_nid, _DT200_CLOCK_SRC)), '');
  if (_clockSource == 'INT') {
    _clockFreq = if_error(data(DevNodeRef(_nid, _DT200_CLOCK_DIV)), 0);
    _clk = make_range(*, *, 1.0D0/_clockFreq);
  }
  else {
    _clk = DevNodeRef(_nid, _DT200_DIs + _DI_NUMBERS[_clockSource]*_DT200_NODES_PER_DI);
  } 

/* should be able to ask the board getEVENT AI E1 and then look it up but
   for now assume the board did what we asked */

  _trigSource = if_error(data(DevNodeRef(_nid, _DT200_TRIG_SRC)), '');
  _trigger = DevNodeRef(_nid, _DT200_DIs + _DI_NUMBERS[_trigSource]*_DT200_NODES_PER_DI);

  /*************************************
   Get setup info
  *************************************/
  _num_chans=MdsValue('Dt200GetNumChans($)',_board);
  _pre_trig = MdsValue("Dt200GetInt($, 'getPhase AI P1 actual-samples')",_board);
  _post_trig = MdsValue("Dt200GetInt($, 'getPhase AI P2 actual-samples')", _board)-1;

  _first_idx = - _pre_trig;
  _last_idx = _post_trig;
  _max_samples = _pre_trig+_post_trig;

  /***********************************************************
   For each channel:
      If channel is turned on
        Get bounds based on startidx and endidx nodes
        Get the increment and filter coefs
        Read the data usng the start, end, inc, and filter coefs
        Define the dimension
        Write the signal
      endif
   Endfor
  ************************************************************/       
  _offset = 0.0;

  for (_chan=0; _chan < _num_chans; _chan++)
  {
    _chan_offset = _chan * _DT200_NODES_PER_AI + _DT200_AI_CHANS;
    _chan_nid = DevHead(_nid) + _chan_offset;
    if (DevIsOn(_chan_nid))
    {
      _startidx = DevNodeRef(_nid,_chan_offset+_DT200_AI_START);
      _endidx   = DevNodeRef(_nid,_chan_offset+_DT200_AI_END);
      _lbound = if_error(long(data(_startidx)),_first_idx);

      _ubound = if_error(long(data(_endidx)),_post_trig); /* or _post_trig -1 ? */
      _inc = if_error(long(data(DevNodeRef(_nid, _chan_offset+_DT200_AI_INC))), 1);
      _filter_coefs = if_error(float(data(DevNodeRef(_nid, _chan_offset+_DT200_AI_COEFFS))), 1.0);

      write(*, "MdsValue('Dt200ReadChannel("//_board//","//_chan+1//","//_lbound-_first_idx//","//_ubound-_first_idx//","//_inc//",_filter_coefs)')");
      _data= MdsValue('Dt200ReadChannel($,$,$,$,$,$)', _board, _chan+1, _lbound-_first_idx, _ubound-_first_idx, _inc, _filter_coefs);     

      write(*, "Read the data "//size(_data));

      if (_inc > 1) {
        _slope = IF_ERROR(SLOPE_OF(_clk), 0);
        if (_slope != 0) {
          write (*, "building new dim with "//_slope);
          _start = (begin_of(_clk) != $missing) ? begin_of(_clk)+(_slope*_inc)/2. : $missing;
          _dim = make_dim(make_window(_lbound/_inc, _ubound/_inc, _trigger), make_range( _start,  *, _slope*_inc));
        } else {
          _dim = make_function(builtin_opcode('MAP'), make_dim(make_window(_lbound, _ubound, _trigger), _clk), make_range(_lbound + _inc/2,  _ubound,  _inc));
        }
      } else {
        _dim = make_dim(make_window(_lbound,_ubound,_trigger),_clk);
      }
      WRITE(*, "About to write channel "//_chan+1);
      write (*, size(_data));
      DevPutSignalNoBounds(_chan_nid, _offset, 10./(32*1024), _data, _dim);
    }
  }
  /* 
    Tell the remote end to reset all the tdi variables etc...
  */
  _dummy = MdsValue('reset_public(),reset_private()');
  return(1);
}
