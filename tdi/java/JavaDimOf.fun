FUN PUBLIC JavaDimOf(in _y1, in _y2)
{
	_already_resampled = 0;
	if_error(_already_resampled = XTreeShr->XTreeTestTimedAccessFlag(), _already_resampled = 0);
	if(_already_resampled)
	{
		return(dim_of(_y1));
	}
	else
	{
		return(dim_of(_y2));
	}
}


