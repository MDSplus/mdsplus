public fun CamGetStat()
{
  _iosb = zero(4,0UW);
  _dummy = RemCamShr->CamGetStat(ref(_iosb));
  return(_iosb);
}
