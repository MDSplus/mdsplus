public fun a12__store(as_is _nid, optional _method)
{
  _name      = DevNodeRef(_nid,1);
  _ext_clock = DevNodeRef(_nid,3);
  _trigger   = DevNodeRef(_nid,4);

  /***********************************
   See if digitizer is done digitizing
  ************************************/
  DevCamChk(_name,CamPiow(_name,0,8,_d=0,16),1,*);
  if (!CamXandQ())
  {
    write(*,'%CAMERR, module '//_name//', Device not triggered');
    return(0);
  }

  /*************************************
   Get setup info
  *************************************/
  DevCamChk(_name,CamPiow(_name,0,0,_setup,16),1,*);
  _offsets = _setup & 0x3f;
  _clk = ((_setup & 0x80) == 0) ? _ext_clock : build_range(*,*,1E-5);

  /************************************************************
   Check module id for max samples and channel selection method
  ************************************************************/

  DevCamChk(_name,CamPiow(_name,0,6,_module_id, 16),1,1);
  _max_samples = (_module_id & 128) ? 32767 : 8192;

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
  for (_chan=0; _chan < 6; _chan++)
  {
    _chan_offset = _chan * 3 + 5;
    _chan_nid = DevHead(_nid) + _chan_offset;
    if (DevIsOn(_chan_nid))
    {
      _startidx = DevNodeRef(_nid,_chan_offset+1);
      _endidx   = DevNodeRef(_nid,_chan_offset+2);
      _lbound = if_error(long(data(_startidx)),0);
      _lbound = min(max(_lbound,0),_max_samples-1);
      _ubound = if_error(long(data(_endidx)),_max_samples-1);
      _ubound = min(max(_ubound,_lbound),_max_samples-1);
      if (((_module_id >> 8) & 0xff) <= 4)
      {
        _chan_sel = 7 - _chan;
      }
      else
      {
        DevCamChk(_name,CamPiow(_name,1,24,_d=0,16),1,1);
        _chan_sel = _chan;
      }
      _offset = ((1 << _chan) & _offsets) != 0 ? -2048 : 0;
      DevCamChk(_name,CamPiow(_name,_chan_sel,16,_d=0,16),1,*);
      DevCamChk(_name,CamFStopw(_name,0,2,_max_samples,_data,16),1,*);
      _dim = make_dim(make_window(_lbound,_ubound,_trigger),_clk);
      DevPutSignal(_chan_nid, _offset, 10./4096, _data[_lbound : _ubound], _lbound, _ubound, _dim);
    }
  }
  return(1);
}
