public fun CamX(optional _iosb)
{
  _istring = present(_iosb) ? '_iosb)' : 'val(0))';
  return(execute(CamImage()//'->CamX('//_istring));
}
