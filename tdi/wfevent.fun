public fun wfevent(in _event, optional _bufsiz)
{
  _data = 0;
  _blen = present(_bufsiz) ? _bufsiz : 4096;
  _dlen = 0;
  _data=zero(_blen,0bu);
  _status = MdsShr->MDSWfevent(_event,_blen,ref(_data),ref(_dlen));
  if (_dlen > _blen) _dlen = _blen;
  _data=_data[0 : (_dlen-1)];
  return(_data);
}
