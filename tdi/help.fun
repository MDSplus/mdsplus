public fun help(optional in _fun, optional in _quiet, optional in _call)
{
/*
        Extract first comment from TDI FUN (including comment markers)

call:   _files = HELP(optional in _fun, optional in _quiet, optional in _call)

        _files:          = list of found files
        _fun:            = name of TDI fun. Wildcards permitted. Default is "*"
        _quiet:          = if set, text is not written to SYS$OUTPUT
        _call:          = if set looks for "call:" lines in help
*/
  if (!present(_fun)) _fun = "*";
        _wild = (scan(_fun,'*') >= 0);
  _quiet = present(_quiet) ? _quiet : 0;
  _call = present(_call) ? _call : 0;
  _ctx = 0Q;
  _file = repeat(' ',512);
  _text = *;
  _n = 0;
  while (_s=MdsShr->LibFindFileRecurseCaseBlind(DESCR("MDS_PATH:"//_fun//".fun"),DESCR(_file),REF(_ctx)))
  {
    _n++;
    _f = trim(_file);
    _text = [_text,_f];
    if (!_quiet) 
    {
     write(*,_f);
/* Now go and find the comment */
    _in_comment = 0;
    _called = 0;
    _lun=fopen(_f,'r');
    while (sizeof(_lun) != 0 && (if_error((_l=Read(_lun),1),0) != 0))
    {
      if (_in_comment)
      {
        if ((_i = MdsShr->StrPosition(DESCR(_l),DESCR("*/"),0)) != 0)
        {
          _l = extract(0,_i+1,_l);
          if (!_quiet) write(,_l);
          break;
        }
        else
        {
        if(len(_l) == 0) _l = " ";
          if(_call > 0) {
                if(scan(_l,'call:') == 0) {
                        _called = 1;
                        if (!_quiet) write(,_l); }
                else
                        if(_called > 0) break;
                }
        else
        if(_wild > 0) {
/* Check if line only contains given characters */
          if (verify(_l,'/*     ') >= 0)
                { write(,_l);
                  break; }
                }
        else
          if (!_quiet) write(,_l);
        }
      }
      else
      {
        if ((_i = MdsShr->StrPosition(DESCR(_l),DESCR("/*"),0)) != 0)
        {
          _l = extract(_i-1,len(_l),_l);
          if ((_i = MdsShr->StrPosition(DESCR(_l),DESCR("*/"),0)) != 0)
          {
            _l = extract(0,_i+1,_l);
            if (!_quiet) write(,_l);
            break;
          }
          else
          {
        if(len(_l) == 0) _l = " ";
          if(_call > 0) {
                if(scan(_l,'call:') == 0) {
                        _called = 1;
                        if (!_quiet) write(,_l); }
                else
                        if(_called > 0) break;
                }
        else
        if(_wild > 0) {
/* Check if line only contains given characters */
          if (verify(_l,'/*     ') >= 0)
                { write(,_f);
                  write(,_l);
                  break; }
                }
        else
            if (!_quiet) write(*,_l);
            _in_comment = 1;
          }
        }
      }
    }
    fclose(_lun);
   }    /* end of quiet option */
  }
  if (size(_text)==0) 
  {
    write(,"No matching FUN's found.");
    _text = ["No matching FUN's found."];
  }
  return(_text);
}