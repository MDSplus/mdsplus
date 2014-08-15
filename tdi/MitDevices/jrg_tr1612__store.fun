public fun jrg_tr1612__store(as_is _nid, optional _method) {
/*
 * $Id$
 *
 * TDI function to read the Joerger TR digitizers
 * (here named JRG_TR1612 - 16 channels, 12 bits)
 *
 * BAN/MBF 23-SEP-2003	Initial Version
 * BAN/MBF 22-JUL-2004  Rewrote to use same method as manual, p.35
 *
 * Copyright (c) 2004
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
DevCamChk(_name,CamPiow(_name,0wu,8wu,_d=0wu,24wu),1,*);
if (!CamXandQ())
  {
  write(*,'%CAMERR, module '//_name//', Device not triggered');
  return(0);
  };

/* Clear the LAM*/	 
DevCamChk(_name,CamPiow(_name,0wu,10wu,_d=0wu,24wu),1,1);

/* Read Module ID */       
DevCamChk(_name,CamPiow(_name,15wu,0wu,_module_id,24wu),1,1);

/*
 * Serial numbers 1 and 2 don't read their MODULE ID correctly;
 *	they need to have the bits set for 40 MHz (bit 14)
 *	and for 256 KB (bit 12)
 * this is a kludge for them until we get them reprogrammed,
 *	but should still work when they are fixed
 * BAN 20-JUL-2004
 */
switch (_module_id & 0x1FF) {
  case( 1 ) _module_id = _module_id | 0x2800; break;
  case( 2 ) _module_id = _module_id | 0x2800; break;
  }

/* Store Module ID */       
DevPutValue( DevHead(_nid) + _module_id_nid, _module_id );

/* Determine module memory size from the module ID bits 12-13
 *	=> mask with 0x1800 (bits 12 and 13 set) shift right 11 bits
 */
_memory_size = [ 65536, 262144, 524288, 1048576 ][ (_module_id & 0x1800) >> 11 ];

/* Check if _samples > _memory_size and set it to _memory_size if it is. */
if (_samples > _memory_size) {
  write(*,'%CAMERR, module '//_name//', Samples to store > module memory.');
  _samples = _memory_size;
  };

/* get the setup of the clock */
if (getnci(_ext_clock,"LENGTH") != 0) _clock = _ext_clock; /* clock is external */
else {
  if (_samp_interval == 0.0) write(*,'%CAMERR, module '//_name//', External Clock not specified');
  else _clock = make_range(*,*,make_with_units(_samp_interval,"s"));
  };

/* Read oldest data sample */
DevCamChk(_name,CamPiow(_name,9wu,0uw,_oldest_data_sample,24wu),1,1);

/* Read number of post trigger samples */
DevCamChk(_name,CamPiow(_name,3wu,0wu,_num_post_trigger_samples,24wu),1,1);

/* for each active channel... */       
for (_input_chan=1; _input_chan <= _active_chans; _input_chan++) {
  _input_chan_nid = (_input_chan-1) * (_num_nodes_per_input+1) + _nid_first_input_node;
  _chan_nid = DevHead(_nid) + _input_chan_nid;
  /* if channel is on */
  if (DevIsOn(_chan_nid)) {
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
    if (_samples == _memory_size) {
      /* Read all samples */
      _data = jrg_tr1612_24_bit_data_read(_name,_input_chan-1wu,0,_memory_size-1);
      /* shift the data values if we don't start at 0 */
      if (_oldest_data_sample <> 0)
        _data = [ _data[_oldest_data_sample : *], _data[0 : _oldest_data_sample-1] ];
      }
    else if (_oldest_data_sample < _samples) {
      /* Data have wrapped around, so read in two chunks and concatenate */
      _first = _memory_size + _oldest_data_sample - _samples - 1;
      _last  = _memory_size - 1;
      _data1 = jrg_tr1612_24_bit_data_read(_name,_input_chan-1wu,_first,_last);
      _first = 0;
      _last  = _oldest_data_sample - 1;
      _data2 = jrg_tr1612_24_bit_data_read(_name,_input_chan-1wu,_first,_last);
      _data  = [ _data1, _data2 ];
      }
    else
       /* Data are in one chunk */
       _data = jrg_tr1612_24_bit_data_read(_name,_input_chan-1wu,
		_oldest_data_sample-_samples,_oldest_data_sample-1);

    /* Write data to tree */
    DevPutSignal(_chan_nid, _offset, _coefficient, _data, _lbound, _ubound, _dim);
    }
  }

return(1);  /* return nicely */
}
