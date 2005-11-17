public fun TR10HWReadChan(in _handle, in _chan, in _start_idx, in _end_idx, in _pts, optional inout _status)
{
	
	private _2M = 2097152;


/* Get current sample */	
	_act_sample = long(0);
	TR10->TR10_Trg_GetActShotSamples(val(_handle), ref(_act_sample));



	_act_pts = long(0);
	TR10->TR10_Trg_GetActPostSamples(val(_handle), ref(_act_pts));
	if(_act_pts < _pts - 32)
	{
	    write(*, "LESS PTS THAN EXPECTED!!!!!: ", _act_pts, _pts);
	}


	_pts = _act_pts;	

	_act_sample = _act_sample - _pts + _start_idx;
	if(_act_sample < 0)
	{
	    write(*, "LESS DATA THAT EXPECTED!!!!!: ", _act_sample, _pts - _start_idx);
	    _act_sample = 0;
	}



/* Allocate buffer */

/* Make sure starting address and data length are multiple of 32 */
	_start_ofs = mod(_act_sample, 32);
	_act_sample = _act_sample - _start_ofs; 

	/*_n_samples = long(_end_idx - _start_idx  + _start_ofs);*/
	_n_samples = long(_pts - _start_idx  + _start_ofs);
	

	_data = zero(_n_samples+32, 0W);

/* Read channel */
	_status_out = TR10->TR10_Mem_Read_DMA(val(_handle), val(byte(_chan)), val(long(_act_sample)), ref(_data), val(_n_samples));

        if( present(_status) )
	   _status = _status_out;

	_out = _data[ _start_ofs : (_end_idx - _start_idx + _start_ofs) ];

	return( _out );

}

	

