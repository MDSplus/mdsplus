public fun Dt200GetSetup(in _board, out _num_chans, out _pre_trig, out _post_trig)
{
  _num_chans=Dt200GetNumChans(_board);
  _pre_trig = Dt200GetInt(_board, 'getPhase AI P1 actual-samples');
  _post_trig = Dt200GetInt(_board, 'getPhase AI P2 actual-samples');
  return(1);
}
