public fun Dt200GetInternalClock(in _board)
{
  _ans = Dt200WriteMaster(_board, "getInternalClock");
  if (extract(0,6,_ans) != 'ACQ32:') {
    Write (*, "Non ACQ32 response from board\n/"//_ans//"/\n");
    abort();
  }

  if(extract(6,17,_ans) != "getInternalClock=") {
    Write (*, "Non getNumSamples response from board\n/"//_ans//"/\n");
    abort();
  }
  _numstr=extract(23, len(_ans)-22, _ans);
  _numstr=extract(0, index(_numstr, ' '), _numstr);
  _ans = if_error(execute(_numstr), 0);
  return(_ans);
}

