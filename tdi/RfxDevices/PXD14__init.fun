public fun PXD14__init()
{
    _handle = LECROY_PXD114->PXDOpen("PXI7::11::INSTR");
    write(*, _handle);

    _input_impedance = 0L;
    _vert_range = 0L;
    _vert_offset = FLOAT(0);
    _vert_coupling = 1L;
    _trig_source_idx = 4L;
    _trig_level = FLOAT(1);
    _trig_slope = 0L;
    _trig_coupling = 1L;
    _trig_impedance = 1L;
    _num_segments = 20L;
    _time_per_segment = FLOAT(500E-9);
    _start_time = FLOAT(-200E-9);

    _segment_len = 0L;
    _sample_rate = FLOAT(0);



   _status = LECROY_PXD114->PXDInit(_handle, 
		_input_impedance,	/*0: 50 Ohm, 1: 1MOhm*/
		_vert_range,		/*0:1, 1:20, 2:40*/
		_vert_offset,		
		_vert_coupling, /*0: AC, 1: DC, 2: GND*/
		_trig_source_idx, /* C1, C2, C3, C4, External, External5 */ 
		_trig_level,
		_trig_slope, /*0: positive, 1: negative, 2: window*/
		_trig_coupling,
		_trig_impedance, /*0: 50 Ohm, 1: 1MOhm*/
		_num_segments, 
		_time_per_segment,
		_start_time,
		REF(_segment_len),
		REF(_sample_rate));

    write(*, 'Status: ', _status);
    write(*, 'Segment len: ', _segment_len);
    write(*, 'Sample rate: ', _sample_rate);


    return(0);	

}
			



