public fun jrg_tr1612__store(as_is _nid, optional _method)
{
/*
 * $Id$
 *
 * TDI function to read the Joerger TR digitizers
 * (here named JRG_TR1612 - 16 channels, 12 bits)
 *
 * BAN/MBF 25-SEP-2003
 * Copyright (c) 2003
 * University of Washington
 * All rights reserved
 */

_name	       = DevNodeRef(_nid,1);  /* get name */
_samp_interval = DevNodeRef(_nid,4);  /* get clock sampling interval */
_ext_clock     = DevNodeRef(_nid,5);  /* get external clock */
_trigger       = DevNodeRef(_nid,6);  /* get trigger */
_samples       = DevNodeRef(_nid,7);  /* get number of samples */
_pre_trigger   = DevNodeRef(_nid,8);  /* get number of pre trigger samples */
_active_chans  = DevNodeRef(_nid,9);  /* get number of active channels */

_module_id_nid = 3;   /* NID for storing MODULE_ID value*/

_num_nodes_per_input = 7;
_nid_first_input_node = 20;

/* See if digitizer is done digitizing (check LAM)*/
DevCamChk(_name,CamPiow(_name,0wu,8wu,_d=0wu,16wu),1,*);
if (!CamXandQ())
  {
  write(*,'%CAMERR, module '//_name//', Device not triggered');
  return(0);
  };

/* Clear the LAM*/	 
DevCamChk(_name,CamPiow(_name,0wu,10wu,_d=0wu,16wu),1,1);

/* Read and store Module ID */       
DevCamChk(_name,CamPiow(_name,15wu,0wu,_module_id,16wu),1,1);
DevPutValue( DevHead(_nid) + _module_id_nid, _module_id );

/* Determine module memory size from the module ID bits 12-13
 *	=> mask with 0x1800 (bits 12 and 13 set) shift right 11 bits
 */
_memory_size = [ 65536, 262144, 524288, 1048576 ][ (_module_id & 0x1800 ) >> 11 ];

/* get the setup of the clock */
if (getnci(_ext_clock,"LENGTH") != 0) _clock = _ext_clock; /* clock is external */
else
  {
  if (_samp_interval == 0.0) write(*,'%CAMERR, module '//_name//', External Clock not specified');
  else _clock = make_range(*,*,make_with_units(_samp_interval,"s"));
  };

/* Read last memory location */  
DevCamChk(_name,CamPiow(_name,8wu,9wu,_d=0wu,16wu),1,1);  /* Initialize pointer for last address */
DevCamChk(_name,CamPiow(_name,14wu,1wu,_last_address,16wu),1,1);  /* Read last address */

/* Read number of post trigger samples */
DevCamChk(_name,CamPiow(_name,3wu,0wu,_num_post_trigger_samples,16wu),1,1);

/* for each active channel... */       
for (_input_chan=1; _input_chan <= _active_chans; _input_chan++)
{
  _input_chan_nid = (_input_chan-1) * (_num_nodes_per_input+1) + _nid_first_input_node;
  _chan_nid = DevHead(_nid) + _input_chan_nid;
  if (DevIsOn(_chan_nid)) /* if channel is on */
  {
    _startidx = DevNodeRef(_nid,_input_chan_nid+2);
    _endidx   = DevNodeRef(_nid,_input_chan_nid+3);
        
    _range    = DevNodeRef(_nid,_input_chan_nid+4); /* Range in Volts P-P */
    _offset   = DevNodeRef(_nid,_input_chan_nid+5); /* Offset in counts */
    _offset   = -1 * _offset; /* Offset in counts; inverted for DevPutSignal() */

/*
 * Note: _lbound and _ubound are used if defined from the tree,
 * BUT all _samples are read from the digitizer and those between
 * _lbound and _ubound are written to the INPUT_XX channel.
 * We will want to (or *should*) fix this later.
 * BAN 25-SEP-2003
 */
    _lbound = if_error(long(data(_startidx)), -_pre_trigger);
    _lbound = min(max(_lbound,-_pre_trigger), _samples-_pre_trigger-1);
    _ubound = if_error(long(data(_endidx)), _samples-_pre_trigger-1);
    _ubound = min(max(_ubound,_lbound), _samples-_pre_trigger-1);
    _dim = make_dim(make_window(_lbound,_ubound,_trigger),_clock);
    
    _coefficient = _range / 4095.0;  /* Calculate coefficient as (range/ n-1 counts) */
    /* read out the channel */
    if (_samples == _memory_size)
    {
      /* Read all samples */
      _data = jrg_tr1612_24_bit_data_read(_name,_input_chan-1wu,0,_memory_size-1);
      /* shift the data values for _last_address location */
      if (_last_address <> _memory_size - 1)
        _data = [ _data[_last_address+1 : *], _data[0 : _last_address] ];
    }
    else if (_last_address < _samples)
    {
	_data2 = jrg_tr1612_24_bit_data_read(_name,_input_chan-1wu,0,_last_address);
	_first = _memory_size - 1 - ( _samples - _last_address + 1 );
	_last  = _memory_size - 1;
	_data1 = jrg_tr1612_24_bit_data_read(_name,_input_chan-1wu,_first,_last);
	_data  = [ _data1, _data2 ];
     }
     else
	_data = jrg_tr1612_24_bit_data_read(_name,_input_chan-1wu,
		_last_address-_samples+1,_last_address);
    DevPutSignal(_chan_nid, _offset, _coefficient, _data, _lbound, _ubound, _dim);
  }
}

return(1);  /* return nicely */
}
