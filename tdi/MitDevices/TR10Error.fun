public fun TR10Error(in _status)
{
  _lstat = - _status;
  _err='                                                                      ';
  if(_status != 0) {
    _stat = TR10->TR10_ErrorToString(val(_lstat), ref(_err), val(len(_err)));
    if (_status !=0) 
    {
      _err = 'TR10: Can not translate error code '//_stat;
    }
    write(*, _err);
    abort();
  }
}
