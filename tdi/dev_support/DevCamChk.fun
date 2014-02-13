public fun DevCamChk(in _name, in _status, optional _x, optional _q, optional _noabort)
{
  /* Check CAMAC operation for successful completion */
  if (!(_status & 1))
  {
    write(*, public _last_device_error = '%CAMERR, module '//_name//', bad status = '//TEXT(_status)//'\n');
    if (!present(_noabort)) 
      abort();
    else
      return(0);
  }
  else if (CamError())
  {
    _iosb = CamGetStat();
    write(*, public _last_device_error = '%CAMERR, module '//_name//', bad iosb[0] = '//TEXT(_iosb[0])//'\n');
    if (!present(_noabort))
      abort();
    else
      return(0);
  }
  else
  {
    if (present(_x)) 
    {
      if (_x != CamX())
      {
        write(*, public _last_device_error  = '%CAMERR, module '//_name//', no X\n');
        if (!present(_noabort))
          abort();
        else
          return(0);
      }
    }
    if (present(_q))
    {
      if (_q != CamQ())
      {
        write(*, public _last_device_error = '%CAMERR, module '//_name//', no Q\n');
        if (!present(_noabort))
          abort();
        else
          return(0);
      }
    }
  }
  return(1);
}
    
