public fun DevIsOn(in _nid)
{
  return(~getnci(_nid,"STATE"));
}
