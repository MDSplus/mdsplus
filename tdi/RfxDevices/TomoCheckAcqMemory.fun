public fun TomoCheckAcqMemory(in _freq, in _duration, in _maxSamples)  
{
	if(_freq * _duration > _maxSamples)
		return (0);
	return (1);
}
