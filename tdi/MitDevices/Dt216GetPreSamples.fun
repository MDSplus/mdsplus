public fun Dt216GetPreSamples(in _board)
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
  _pre = index(_ans, 'pre=');
  _numstr = extract(_pre+4, len(_ans)-_pre-4, _ans);
  _numstr = extract(0, index(_numstr, ' '), _numstr);
  _ans = if_error(execute(_numstr), 0);
  return(_ans);
}

  
