public fun TR10GetActPostSamples(in _handle)
{
  _samps = 0L;
  TR10Error(TR10->TR10_GetActPostSamples(val(_handle), ref(_samps)));
  return(_samps);
}
