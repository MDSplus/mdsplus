public fun dt101__store(as_is _nid, optional _method)
{
  _DTYPE_RANGE = 201BU;
  _node = DevNodeRef(_nid,1);
  _node = if_error(data(_node), "");
  if (Len(_node) > 0) {
    _status = MdsConnect(_node);
    if (_status < 0) {
      Write(*,"Could not connect to "//_node);
      Abort();
    }
  }
  _board     = DevNodeRef(_nid,2);
  _ext_clock = DevNodeRef(_nid,4);
  _trigger   = DevNodeRef(_nid,5);

  /***********************************
   See if digitizer is done digitizing
  ************************************/

  _state = MdsValue('Dt100WriteMaster($, "getState")', _board);
  if (_state != "ACQ32:0 ST_STOP") {
    write(*, '%DT100ERR, board '//_board//' Device not triggered');
    return(0);
  }

  /*************************************
    Do the DMA Transfer
   ************************************/
  _dummy = MdsValue('Dt100WriteMaster($, "bigdump")', _board);

  /*************************************
   Get setup info
  *************************************/
  _num_chans = MdsValue('Dt100GetNumChannels($)', _board);
  _max_samples = MdsValue('Dt100GetNumSamples($)', _board);

  /**********************************
   for now we can not ask the board for the number
   of post trigger samples and the mode, so get them
   from the tree.
   ***************************************/
  _mode      = DevNodeRef(_nid,9);
  switch (_mode) {
  case (3)
    _post_trig = MdsValue('Dt100GetPostSamples($)', _board);
    _first_idx = -1* MdsValue('Dt100GetPreSamples($)', _board);
     break;
  case (2)
    _active_chans = DevNodeRef(_nid,7);
    _active_chans = min(max(_active_chans, 0), 32);
    _mem_size = DevNodeRef(_nid, 6)/2;
    _chansize = _mem_size*1024*1024 / _active_chans;
    _chansize = if_error(min(_chansize, DevNodeRef(_nid, 8)*1024), _chansize);
    _first_idx = -1*(_max_samples - _chansize);
    break;
  case DEFAULT
    _first_idx = 0;
  }

  /*********************************************************
    fill in the clock with the internal or external clock
    depending on the clock setting.
  **********************************************************/
  _int_clock = if_error(data(DevNodeRef(_nid, 11)), 0);
  if (_int_clock == 0) {
    _clk = DevNodeRef(_nid, 4);
  }
  else {
    _clk = make_range(*, *, 1./_int_clock);
  }

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
    _chan_offset = _chan * 5 + 12;
    _chan_nid = DevHead(_nid) + _chan_offset;
    if (DevIsOn(_chan_nid))
    {
      _startidx = DevNodeRef(_nid,_chan_offset+1);
      _endidx   = DevNodeRef(_nid,_chan_offset+2);
      _lbound = if_error(long(data(_startidx)),_first_idx);
      _lbound = min(max(_lbound,_first_idx),_max_samples-1);
      _ubound = if_error(long(data(_endidx)),_max_samples+_lbound-1);
      _ubound = min(max(_ubound,_lbound),_max_samples+_lbound-1);
      _inc = DevNodeRef(_nid, _chan_offset+3);
      _filter_coefs = DevNodeRef(_nid, _chan_offset+4);
      _inc = if_error(long(data(_inc)), 1);
      _filter_coefs = if_error(float(data(_filter_coefs)), 1.0);
/*
      Write (*, "Calling  Dt101ReadChannel("//_board//","//_chan+1//','//_lbound-_first_idx//','//_ubound-_first_idx//','// _inc//', _filter_coefs');
      _data= MdsValue('Dt101ReadChannel($,$,$,$,$,$)', _board, _chan+1, _lbound-_first_idx, _ubound-_first_idx, _inc, _filter_coefs);
*/
      Write (*, "Calling  Dt101ReadChannel2("//_board//","//_chan+1//','//_lbound//','//_ubound//','// _inc//', _filter_coefs');
      _data= MdsValue('Dt101ReadChannel2($,$,$,$,$,$)', _board, _chan+1, _lbound, _ubound, _inc, _filter_coefs);
      write(*, "Read the data "//size(_data));
      if (_inc > 1) {
        _slope = IF_ERROR(SLOPE_OF(_clk), 0);
        if (_slope != 0) {
          write (*, "building new dim with "//_slope);
          /* NOTE: build/make slope is slope, begin, end not begin, end, slope !! */
          _dim = make_dim(make_window(_lbound/_inc, _ubound/_inc, _trigger), make_slope(_slope*_inc, begin_of(_clk)+(_slope*_inc)/2.,  *));
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
