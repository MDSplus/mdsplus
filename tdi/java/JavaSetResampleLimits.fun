FUN PUBLIC JavaSetResampleLimits(in _xmin, in _xmax, in _dt)
{
	write(*, 'JavaSetResampledLimits', _xmin, _xmax, _dt);	
	TreeShr->TreeSetTimeContext(descr(_xmin),descr(_xmax), descr(_dt));
	XTreeShr->XTreeResetTimedAccessFlag();
}
