public fun ndims(as_is _sigin)
{
  _sig = _sigin;
  while ( execute("kind(`_sig)") <> 195) _sig = execute("`value_of(_sig)");
  return(execute("ndesc(`_sig)")-2);
}
