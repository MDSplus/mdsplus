public fun SetTimeContext(optional _start, optional _end, optional _delta)
{
  _exp="TreeShr->TreeSetTimeContext(";
  _exp=_exp//((present(_start))?"descr(_start),":"val(0),");
  _exp=_exp//((present(_end)) ? "descr(_end),":"val(0),");
  _exp=_exp//((present(_delta)) ? "descr(_delta))":"val(0))");
  return(execute(_exp));
}

    
