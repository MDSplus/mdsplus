public fun wfevent(in _event, optional _bufsiz, optional _timeout)
{
  _data = 0;
  _blen = present(_bufsiz) ? _bufsiz : 4096;
  _dlen = 0;
  _data=zero(_blen,0bu);
  _timeout = present(_timeout) ? _timeout : 0;
  _status = MdsShr->MDSWfeventTimed(_event,val(_blen),ref(_data),ref(_dlen),val(_timeout));
  if (_status & 1) {
    if (_dlen > _blen) _dlen = _blen;
      _data=_data[0 : (_dlen-1)];
  } else {
   _data=*;
  }
  return(_data);
}
