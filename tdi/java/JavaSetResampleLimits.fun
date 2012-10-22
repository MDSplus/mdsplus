FUN PUBLIC JavaSetResampleLimits(in _xmin, in _xmax, in _dt)
{
	write(*, 'JavaSetResampledLimits', _xmin, _xmax, _dt);	
	public _jscope_xmin = _xmin;
	public _jscope_xmax = _xmax;
	public _jscope_dt = _dt;
	TreeShr->TreeSetTimeContext(descr(_xmin),descr(_xmax), descr(_dt));
	XTreeShr->XTreeResetTimedAccessFlag();

}
