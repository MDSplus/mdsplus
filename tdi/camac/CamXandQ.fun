public fun CamXandQ(optional _iosb)
{
  _istring = present(_iosb) ? '_iosb)' : 'val(0))';
  return(execute(CamImage()//'->CamXandQ('//_istring));
}
