public fun Dt216GetPostSamples(in _board)
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
  _post = index(_ans, 'post=');
  _numstr = extract(_post+5, len(_ans)-_post-5, _ans);
  _end = index(_numstr, ' ');
  if (_end > 0) {
    _numstr = extract(0, _end, _numstr);
  }
  _ans = if_error(execute(_numstr), 0);
  return(_ans);
}

  
