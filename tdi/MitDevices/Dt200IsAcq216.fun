public fun Dt200IsAcq216(in _board)
{
  _ans = Dt200WriteMaster(_board, "getFwrev");
  if (extract(0,6,_ans) != 'ACQ32:') {
    Write (*, "Non ACQ32 response from board\n/"//_ans//"/\n");
    abort();
  }
  return(extract(6,10,_ans) == "ACQ216CPCI");
}
