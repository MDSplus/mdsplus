public fun PXD114__init(as_is _nid, optional _method)
{
	private _N_HEAD = 0;
	private _N_NAME = 1;
	private _N_COMMENT = 2;
	private _N_TRIG_SOURCE = 3;
	private _N_TRIG_LEVEL = 4;
	private _N_TRIG_EDGE = 5;
	private _N_TRIG_COUPL = 6;
	private _N_TRIG_IMP = 7;
	private _N_TRIG_TIME = 8;
	private _N_NUM_SEGMENTS = 9;
	private _N_SEGMENT_TIME = 10;
	private _N_SEG_TIME_OFS = 11;
	private _N_ACT_SEG_LEN = 12;
	private _N_ACT_SAMP_FRQ = 13;

	private _N_CHANNEL_0 = 14;

	private _K_NODES_PER_CHANNEL = 6;
	private _N_CHAN_INPUT_IMP = 1;
	private _N_CHAN_RANGE = 2;
	private _N_CHAN_OFFSET = 3;
	private _N_CHAN_COUPLING = 4;
	private _N_CHAN_DATA = 5;

	private _INVALID = 10E20;

	write(*, 'Start PDX114 INIT');

	_board_id = if_error(data(DevNodeRef(_nid, _N_NAME)), 'INVALID');
	if(_board_id == 'INVALID')
	{
    	DevLogErr(_nid, "Invalid board name specification");
		abort();
	}

	DevNodeCvt(_nid, _N_TRIG_SOURCE, ['C1', 'C2','C3','C4','EXTERNAL','EXTERNAL5'], [0, 1,2,3,4,5], _trig_source = 4);
	_trig_level = if_error(data(DevNodeRef(_nid, _N_TRIG_LEVEL)), _INVALID);
	if(_trig_level == _INVALID)
	{
    	DevLogErr(_nid, "Invalid trigger level specification");
		abort();
	}

	DevNodeCvt(_nid, _N_TRIG_EDGE, ['POSITIVE', 'NEGATIVE'], [0, 1], _trig_edge = 0);
	DevNodeCvt(_nid, _N_TRIG_COUPL, ['DC', 'AC', 'GND'], [0, 1,2], _trig_coupl = 1);
	DevNodeCvt(_nid, _N_TRIG_IMP, [50,1000000], [0, 1], _trig_imp = 1);

	_trig_time = if_error(data(DevNodeRef(_nid, _N_TRIG_TIME)), _INVALID);
	if(_trig_time == _INVALID)
	{
    	DevLogErr(_nid, "Invalid trigger time specification");
		abort();
	}
	_num_segments= if_error(data(DevNodeRef(_nid, _N_NUM_SEGMENTS)), _INVALID);
	if(_num_segments == _INVALID || _num_segments <= 0)
	{
    	DevLogErr(_nid, "Invalid number of segments specification");
		abort();
	}
	_segment_time= if_error(data(DevNodeRef(_nid, _N_SEGMENT_TIME)), _INVALID);
	if(_segment_time == _INVALID || _segment_time <= 0)
	{
    	DevLogErr(_nid, "Invalid segment time specification");
		abort();
	}
	_seg_time_ofs= if_error(data(DevNodeRef(_nid, _N_SEG_TIME_OFS)), _INVALID);
	if(_seg_time_ofs == _INVALID)
	{
    	DevLogErr(_nid, "Invalid segment time offset specification");
		abort();
	}

    _handle = LECROY_PXD114->PXDOpen(_board_id);
	if(_handle == 0)
	{
    	DevLogErr(_nid, "Cannot open device "// _board_id);
		abort();
	}




	for(_c = 0; _c < 4; _c++)
	{
		
		DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_INPUT_IMP, 
				[50,1000000],[0,1], _chan_input_imp = 1);
		DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE, 
				[0.04,0.08,0.16,0.4,0.8,1.6,4,8],[0,1,2,3,4,5,6,7], _chan_range = 0);
		_chan_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)), _INVALID);
		if(_chan_offset == _INVALID)
		{
			DevLogErr(_nid, "Invalid channel offset specification for channel "//(_c+1));
			abort();
		}
		DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_COUPLING, 
				['DC', 'AC', 'GND'],[0,1,2], _chan_coupling = 0);


		write(*, 'Input Imp.: ', _chan_input_imp);
		write(*, 'Range: ', _chan_range);
		write(*, 'Offset: ', _chan_offset);
		write(*, 'Coupling: ', _chan_coupling);



		_status = LECROY_PXD114->PXDConfigureChannel(_handle, _c, _chan_input_imp, _chan_range, 
			FLOAT(_chan_offset), _chan_coupling);

		if(_status != 1)
		{
			DevLogErr(_nid, "Channel configuration failed for channel "//(_c+1));
			abort();
		}
	}



	_status = LECROY_PXD114->PXDConfigureTrigger(_handle, _trig_source, FLOAT(_trig_level), _trig_edge, 
		_trig_coupl, _trig_imp);

	write(*, 'Trig source: ', _trig_source);
	write(*, 'Trig level: ', _trig_level);
	write(*, 'Trig edge: ', _trig_edge);
	write(*, 'Trig coupling: ', _trig_coupl);
	write(*, 'Trig impedance: ', _trig_imp);

		 
	if(_status != 1)
	{
		DevLogErr(_nid, "Trigger configuration failed");
		abort();
	}

	_act_seg_len = 0L;
	_act_samp_frq = FLOAT(0);

	_status = LECROY_PXD114->PXDConfigureAcquisition(_handle, LONG(_num_segments), FLOAT(_segment_time),
		FLOAT(_seg_time_ofs), REF(_act_seg_len), REF(_act_samp_frq));

	write(*, 'Num segments: ', _num_segments);
	write(*, 'Segment time: ', _segment_time);
	write(*, 'Segment time offset: ', _seg_time_ofs);
/*	_act_seg_len = _segment_time * 1E9;
	_act_samp_frq = 1E9;
	_status = 1;

*/
		 
	if(_status != 1)
	{
		DevLogErr(_nid, "Acquisition configuration failed");
		abort();
	}

    write(*, 'Segment len: ', _act_seg_len);
    write(*, 'Sample rate: ', _act_samp_frq);

	DevPut(_nid, _N_ACT_SEG_LEN, _act_seg_len);
	DevPut(_nid, _N_ACT_SAMP_FRQ, FT_FLOAT(_act_samp_frq));

write(*, 'End PDX114 INIT');
    return(1);	

}
			



