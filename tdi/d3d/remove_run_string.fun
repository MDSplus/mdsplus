/* Fix the run string from PPPL by removing the run_ part and
   leaving only the shot number.
*/
public fun remove_run_string(in _runstring)
{

  _len = len(_runstring);
  if (_len < 5) {
    write(*, "\nERROR: _runstring is too short.\n");
    abort();
  }

  _run = extract(4,_len-4,_runstring);

  return(_run);

}
