public fun FramesReshape(in _frame_sig)
{
	_frames = data( _frame_sig );

	_dims = shape(_frames);

	write(*, _dims);

	_num_row = _dims[1];

	write(*, _num_row);
	
	_ndims = size(_dims);
	
	_frames_reshape = _frames;
	
	if(_ndims == 3)
	{
		_frames_reshape = _frames[*,int([0 : (_num_row+0.5) : 0.5]), *];
	}

	if(_ndims == 2)
	{
		_frames_reshape = _frames[*,int([0 : (_num_row+0.5) : 0.5])];
	}
	
	return (make_signal(_frames_reshape,, dim_of(_frame_sig) ) );

}
