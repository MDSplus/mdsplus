public fun Dt200GetInt(in _board, in _cmd)
{
  _ans = Dt200WriteMaster(_board, _cmd);
  if (extract(0,6,_ans) != 'ACQ32:') {
    Write (*, "Non ACQ32 response from board\n/"//_ans//"/\n");
    abort();
  }
  _num_str = extract(6, len(_ans)-6, _ans);
  _ans = if_error(execute(_num_str), 0);
  return(_ans);
}
