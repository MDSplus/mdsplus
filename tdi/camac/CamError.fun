public fun CamError(optional _xexpected, optional _qexpected, optional _iosb)
{
  _xstring = present(_xexpected) ? 'long(_xexpected),' : 'val(0),';
  _qstring = present(_qexpected) ? 'long(_qexpected),' : 'val(0),';
  _istring = present(_iosb) ? '_iosb)' : 'val(0))';
  return(execute(CamImage()//'->CamError('//_xstring//_qstring//_istring));
}
