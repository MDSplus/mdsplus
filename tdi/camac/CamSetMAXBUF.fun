public fun CamGetMAXBUF(in _name, in _size)
{
  return(CamShr->CamSetMAXBUF(_name,val(_size)));
}
