public fun ndims(as_is _sigin)
{
/* ndims(signal) returns number of dimensions defined in signal structure
   ndims(array) returns rank of array
   ndims(scalar) returns 0
*/
  _iterations=0;
  _sig = _sigin;
  while ( (execute("class(`_sig)") == 194) && (execute("kind(`_sig)") <> 195) && _iterations < 50)
  { _sig = execute("`value_of(_sig)");
    _iterations++;
  }
  if (execute("kind(`_sig)") == 195)
    return(execute("ndesc(`_sig)")-2);
  else
    return(execute("rank(`_sig)"));
}
