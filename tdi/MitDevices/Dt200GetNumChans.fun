public fun Dt200GetNumchans(in _board)
{
  _ans = Dt200WriteMaster(_board, "getNumChannels");
  if (extract(0,6,_ans) != 'ACQ32:') {
    Write (*, "Non ACQ32 response from board\n/"//_ans//"/\n");
    abort();
  }
  if(extract(6,15,_ans) != "getNumChannels=") {
    Write (*, "Non getNumSamples response from board\n/"//_ans//"/\n");
    abort();
  }
  _numstr=extract(21, len(_ans)-20, _ans);
  _ans = if_error(execute(_numstr), 0);
  return(_ans);
}
