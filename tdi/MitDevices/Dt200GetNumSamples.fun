public fun Dt200GetNumSamples(in _board)
{
  _ans = Dt200WriteMaster(_board, "getNumSamples");
  if (extract(0,6,_ans) != 'ACQ32:') {
    Write (*, "Non ACQ32 response from board\n/"//_ans//"/\n");
    abort();
  }
  if(extract(6,14,_ans) != "getNumSamples=") {
    Write (*, "Non getNumSamples response from board\n/"//_ans//"/\n");
    abort();
  }
  _numstr=extract(20, len(_ans)-20, _ans);
  _sp = index(_numstr, ' ');
  if (_sp > 0) {
    _numstr = extract(0, _sp, _numstr);
  }
  _ans = if_error(execute(_numstr), 0);
  return(_ans);
}

  
