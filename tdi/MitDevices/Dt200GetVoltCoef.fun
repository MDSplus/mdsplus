public fun Dt200GetVoltCoef(in _board)
{
  _ans = Dt200WriteMaster(_board, "getVoltsRange");
  if (extract(0,6,_ans) != 'ACQ32:') {
    Write (*, "Non ACQ32 response from board\n/"//_ans//"/\n");
    abort();
  }
  if(extract(6,17,_ans) != "getVoltsRange AI=") {
    Write (*, "Non getVoltsRange response from board\n/"//_ans//"/\n");
    Write (*, "the chunk in question was/"//extract(6,23,_ans)//"/\n");
    abort();
  }
  _numstr=extract(23, len(_ans)-23, _ans);
  _vidx = index(_numstr, 'V');
  if (_vidx > 0) {
    _numstr1 = extract(0, _vidx, _numstr);
  }
  _start = if_error(execute('float('//_numstr1//')'), -10.0);
  _numstr2 = extract(_vidx+2, len(_numstr)-_vidx-1, _numstr);
  _vidx = index(_numstr2, 'V');
  _numstr2 = extract(0, _vidx, _numstr2);
  _end = if_error(execute('float('//_numstr2//')'), 10.0);
  _coef = (_end - _start)/(2^16-1);
  return(_coef);
}

  
