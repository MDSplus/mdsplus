public fun TR10HWReadChan(in _handle, in _chan, in _start_idx, in _end_idx, in _pts)
{
	
	private _2M = 2097152;



/* Get current sample */	
	_act_sample = long(0);
	TR10->TR10_Trg_GetActShotSamples(val(_handle), ref(_act_sample));

write(*, 'ACT SAMPLE ', _act_sample);

	_act_sample = _act_sample - _pts + _start_idx;
	if(_act_sample < 0)
		_act_sample = _act_sample + _2M;

  write(*, 'NUOVO ACT SAMPLE ', _act_sample);
/* Allocate buffer */

/* Make sure starting address and data length are multiple of 32 */
	_start_ofs = mod(_act_sample, 32);
	_act_sample = _act_sample - _start_ofs; 

	_n_samples = long(_end_idx - _start_idx  + _start_ofs);
	if(mod(_n_samples, 32) > 0)
		_n_samples = _n_samples + 32 - mod(_n_samples, 32);
	

	_data = zero(_n_samples, 0L);

write(*, 'MEMREAD', _chan, _act_sample, _n_samples);

/* Read channel */
	TR10->TR10_Mem_Read_DMA(val(_handle), val(byte(_chan)), val(long(_act_sample)), ref(_data), val(_n_samples));


write(*, 'LETTO');
	return(_data[_start_ofs:(_end_idx - _start_idx)]);
}

	

