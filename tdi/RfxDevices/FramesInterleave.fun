public fun FramesInterleave(in _frame_sig)
{

	_frames = data( _frame_sig );
	_times = dim_of(_frame_sig);
	
	_t0 = _times[0];
	_dt = _times[1] - _times[0];

	_dims = shape(_frames);

	_num_col = _dims[0];
	_num_row = _dims[1];
	_num_frame = _dims[2];
		
	_ndims = size(_dims);
	
	if(_ndims == 3)
	{
	      _frames1 = _frames[*, [ 0 : _num_row : 2], *];

	      _framesEven = _frames1[*,int([0 : (_num_row+0.5) : 0.5]), *];

	      _frames1 = _frames[*, [ 1 : _num_row : 2], *];

	      _framesOdd = _frames1[*,int([0 : (_num_row+0.5) : 0.5]), *];
	     
	      _frames_out = [];
	      _times_out = [];	      
	      for(_i = 0; _i < _num_frame; _i++)
	      {
	       	 _times_out = [ _times_out, _t0 + _i*2 * _dt/2., _t0 + (_i*2 + 1) * _dt/2.];
	         _frames_out = [_frames_out, set_range(_num_col, _num_row,_framesEven[*,*,_i]), set_range(_num_col,
		 _num_row, _framesOdd[*,*,_i])];
	      }
	      
	      _frames_odd_even = set_range(_num_col, _num_row, _num_frame * 2,_frames_out);
	    
	}	
	return (make_signal(_frames_odd_even, *, _times_out ) );	

}
