public fun CamQ(optional _iosb)
{
  _istring = present(_iosb) ? '_iosb)' : 'val(0))';
  return(execute(CamImage()//'->CamQ('//_istring));
}
