public fun dt100__store(as_is _nid, optional _method)
{
  _node = DevNodeRef(_nid,1);
  _node = if_error(data(_node), "");
  if (Len(_node) > 0) {
    _status = MdsConnect(_node);
    if (! _status) {
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
  _cmd =   'Dt100WriteMaster('//_board//', "getState")';
  _state = MdsValue(_cmd);
  if (_state != "ACQ32:0 ST_STOP") {
    write(*, '%DT100ERR, board '//_board//' Device not triggered');
    return(0);
  }

  /*************************************
   Get setup info
  *************************************/
  _cmd = 'Dt100GetNumChannels('//_board//')';
  _num_chans = MdsValue(_cmd);
  _cmd = 'Dt100GetNumSamples('//_board//')';
  _max_samples = MdsValue(_cmd);
/*
  _clk = ((_setup & 0x80) == 0) ? _ext_clock : build_range(*,*,1E-5);
*/
  _clk = build_range(*,*,5e-6);
  /***********************************************************
   For each channel:
      If channel is turned on
        Get bounds based on startidx and endidx nodes
        Compute channel selection code based on module id
        Compute data offset based on offsets bit mask
        Select the channel
        Read the data
        Define the dimension
        Write the signal
      endif
   Endfor
  ************************************************************/       
  _offset = 5.;
  for (_chan=0; _chan < _num_chans; _chan++)
  {
    _chan_offset = _chan * 3 + 10;
    _chan_nid = DevHead(_nid) + _chan_offset;
    if (DevIsOn(_chan_nid))
    {
      _startidx = DevNodeRef(_nid,_chan_offset+1);
      _endidx   = DevNodeRef(_nid,_chan_offset+2);
      _lbound = if_error(long(data(_startidx)),0);
      _lbound = min(max(_lbound,0),_max_samples-1);
      _ubound = if_error(long(data(_endidx)),_max_samples-1);
      _ubound = min(max(_ubound,_lbound),_max_samples-1);

      _cmd =  'Dt100ReadChannel('//_board//','// _chan+1//')';
      _data = MdsValue(_cmd);
      _dim = make_dim(make_window(_lbound,_ubound,_trigger),_clk);
      DevPutSignal(_chan_nid, _offset, 10./(32*1024), _data[_lbound : _ubound], _lbound, _ubound, _dim);
    }
  }
  return(1);
}
