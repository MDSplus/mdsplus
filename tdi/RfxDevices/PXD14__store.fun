public fun PXD14__store(in _handle)
{
   /* _handle = LECROY_PXD114->PXDOpen("PXI7::11::INSTR");*/
    write(*, _handle);

    _num_segments = 20L;
    _segment_len = 502L;
    _seg1 = zero(_num_segments*_segment_len, 0W);
    _seg2 = zero(_num_segments*_segment_len, 0W);
    _seg3 = zero(_num_segments*_segment_len, 0W);
    _seg4 = zero(_num_segments*_segment_len, 0W);

    _trig_times = zero(_num_segments, FT_FLOAT(0));




    _status = LECROY_PXD114->PXDStore(
		long(_handle), 
		_segment_len,
		_num_segments,
		ref(_trig_times), 
		ref(_seg1),
		ref(_seg2),
		ref(_seg3),
		ref(_seg4));
   write(*, 'Status: ', _status);

    if(_status == 1)
    	write(*, 'Trig times: ', _trig_times);

    return(0);	

}
			



