public fun DevCamChk(in _name, in _status, optional _x, optional _q)
{
  /* Check CAMAC operation for successful completion */

  if (!_status)
  {
    write(*,'%CAMERR, module '//_name//', bad status = '//TEXT(_status)//'\n');
    abort();
  }
  else if (CamError())
  {
    _iosb = CamGetStat();
    write(*,'%CAMERR, module '//_name//', bad iosb[0] = '//TEXT(_iosb[0])//'\n');
    abort();
  }
  else if (present(_x)) 
  {
    if (_x != CamX())
    {
      write(*,'%CAMERR, module '//_name//', no X\n');
      abort();
    }
  }
  else if (present(_q) /* && _q != CamQ() */)
  {
    if (_q != CamQ())
    {
      write(*,'%CAMERR, module '//_name//', no Q\n');
      abort();
    }
  }
  return(1);
}
    
