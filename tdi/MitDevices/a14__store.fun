public fun a14__store(as_is _nid, optional _method)
{
  _debug = 0;
  _name      = DevNodeRef(_nid,1);
  _ext_clock = DevNodeRef(_nid,4);
  _trigger   = DevNodeRef(_nid,5);
  _special_dimension = DevNodeRef(_nid,8);
  _is_special_dimension = (getnci(_special_dimension,"LENGTH") > 0);
  _is_ext_clock = getnci(_ext_clock,"LENGTH") > 0;
  _trigger_name = getnci(_trigger,"MINPATH");
  _ext_clock_name = getnci(_ext_clock,"MINPATH");
  _special_dim_name = getnci(_special_dimension,"MINPATH");

  /***********************************
   See if digitizer is done digitizing
  ************************************/
  DevCamChk(_name,CamPiow(_name,0,1,_rtsr,24),1,*);  /* Get run status */
  if (_debug) write(*,"_rtsr=",_rtsr);
  DevCamChk(_name,CamPiow(_name,2,1,_memptr,24),1,*); /* Get memory pointer */
  if (_debug) write(*,"_memptr=",_rtsr);
  DevCamChk(_name,CamPiow(_name,3,1,_pts,24),1,*); /* Get memory pointer */
  if (_debug) write(*,"_pts=",_pts);
  DevCamChk(_name,CamPiow(_name,4,1,_sr,24),1,*); /* Get status register */
  if (_debug) write(*,"_sr=",_sr);
  DevCamChk(_name,CamPiow(_name,5,1,_range,24),1,*); /* Get ADC ranges */
  if (_debug) write(*,"_range=",_range);
  DevCamChk(_name,CamPiow(_name,1,24,_d=0,24),1,*);  /* Select CMD mode */
  _sr_mode = (_sr >> 8) & 0x7;
  if (_debug) write(*,"_sr_mode=",_sr_mode);
  _sr_big_ram = (_sr >> 15) & 1;
  if (_debug) write(*,"_sr_big_ram=",_sr_big_ram);
  _sr_clock_speed = _sr & 0x7;
  _clock_divide = ([1,2,4,10,20,40,100])[_sr_clock_speed];
  _dt = 1E-6 * _clock_divide;
  if (_debug) write(*,"_dt=",_dt);
  _memsize_idx = (_sr_big_ram ? 512 : 128) * 1024 - 1;
  _pts *= _sr_big_ram ? 2048 : 1024;
  _pts = min(_pts,_memsize_idx);
  if ((_rtsr & 1) && (_sr_mode || (_memptr < _pts)))
  {
    write(*,'%CAMERR, module '//_name//', Device not triggered');
    return(0);
  }
  switch (_sr_mode)
  {
    case(0) {_min_idx = 0;_max_idx = _pts ? _pts : _memsize_idx - 1;_start_addr = 0;break;}
    case(4) {_min_idx = 0;_max_idx = _pts ? _pts : _memsize_idx - 1;_start_addr = 0;break;}
    case(1) {_min_idx = _pts - _memsize_idx; _max_idx = _pts - 1; _start_addr = _memptr - _pts; break;}
    case(2) {_min_idx = 0; _max_idx = _memptr ? _memptr - 1 : _memsize_idx; _start_addr = 0; break;}
    case(3) {_min_idx = 0; _max_idx = _memptr ? _memptr - 1 : _memsize_idx; _start_addr = 0; break;}
    case default {_min_idx = 0; _max_idx = _memsize_idx; _start_addr = 0; break;}
  }
  _clk = _is_ext_clock ? _ext_clock_name : "* : * : "//_dt;
  _dim_str_for_post_mode_p1 = "BUILD_WITH_UNITS((_A14_ADJUST=A14_ADJUST("//_trigger_name//","//_clk//
                                    "),BUILD_DIM(BUILD_WINDOW("//(_sr_mode != 0)//"+";
  _dim_str_for_post_mode_p2 = "+_A14_ADJUST,"//(_sr_mode != 0)//"+";
  _dim_str_for_post_mode_p3 = "+_A14_ADJUST,"//_trigger_name//"),"//_clk//")),'seconds')";
  _burst_dt = _is_ext_clock ? "SLOPE_OF("//_ext_clock_name//")" : _dt;
  _dim_str_for_burst_mode = "BUILD_WITH_UNITS(("//_trigger_name//" : ("//_trigger_name//" + ("//_pts//" - .5) * "//
                            _burst_dt//" * ("//_clock_divide//")) : "//_burst_dt//" * "//_clock_divide//"),'seconds')";
  _dim = compile(_dim_str_for_burst_mode);
  if (_debug) write(*,"_dim_str_for_burst_mode=",_dim_str_for_burst_mode);
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
    _chan_offset = _chan * 3 + 13;
    _chan_nid = DevHead(_nid) + _chan_offset;
    if (DevIsOn(_chan_nid))
    {
      _startidx = DevNodeRef(_nid,_chan_offset+1);
      _endidx   = DevNodeRef(_nid,_chan_offset+2);
      _lbound = if_error(long(data(_startidx)),_min_idx);
      _lbound = min(max(_lbound,_min_idx),_max_idx);
      _ubound = if_error(long(data(_endidx)),_max_idx);
      _ubound = min(max(_ubound,_lbound),_max_idx);
      _samples = _ubound - _lbound + 1;
      _offset = ([0,0,0,0,-2048,-2048,-2048,-2048])[_range & 7];
      _coefficient = ([2.5/4096,5./4096,5./4096,10./4096,5./4096,10./4096,10./4096,20./4096])[_range & 7];
      if (_samples > 0)
      {
        _addr = _start_addr + _lbound;
        _samples_read = 0;
        _data = *;
        DevCamChk(_name,CamPiow(_name,6,16,_chan,16),1,*);
        while (size(_data) < _samples)
        {
          _samples_to_read = min(_samples - size(_data), 32000);
          if (_samples_to_read > 1 && ((_addr + _samples_to_read) & 0xff) == 0xff)
            _samples_to_read--;
          DevCamChk(_name,CamPiow(_name,2,17,_addr,24),1,*);
          DevCamChk(_name,CamFStopw(_name,0,2,_samples_to_read,_data_part,16),1,*);
          _data = [_data,_data_part & 4095];
          _addr += _samples_to_read;
        }
        _data = set_range(_lbound : _ubound, _data);
        if (_is_special_dimension)
          _dim_str = _special_dim_name;
        else if (_sr_mode == 0 || _sr_mode == 1)
          _dim_str = _dim_str_for_post_mode_p1 // _lbound // _dim_str_for_post_mode_p2 // _ubound // _dim_str_for_post_mode_p3;
        else if (_sr_mode == 3)
          _dim_str = _dim_str_for_burst_mode;
        else
          _dim_str = "*";
        if (_debug) write(*,"_dim_str=",_dim_str);
        _dim = compile(_dim_str);
        DevPutSignal(_chan_nid, _offset, _coefficient, _data, _lbound, _ubound, _dim);
      }
    }
  }
  return(1);
}
