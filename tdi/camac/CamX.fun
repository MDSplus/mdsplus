public fun CamX(optional _iosb)
{
  _istring = present(_iosb) ? '_iosb)' : 'val(0))';
  return(execute('RemCamShr->CamX('//_istring));
}
