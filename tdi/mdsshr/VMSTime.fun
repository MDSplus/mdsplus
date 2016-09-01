public fun VMSTime(in _date_str)
{
  _ans = 0qu;
  _stat = MdsShr->LibConvertDateString(_date_str, ref(_ans));
  if (not _stat)
    write(*, 'VMSTime: '//getmsg(_stat));
  return(_ans);
}
