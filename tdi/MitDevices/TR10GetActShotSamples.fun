public fun TR10GetActShotSamples(in _handle)
{
  _samps = 0L;
  TR10Error(TR10->TR10_Trg_GetActShotSamples(val(_handle), ref(_samps)));
  return(_samps);
}
