public fun job_wait(in _name, in _shot, optional in _timeout)
{
  return(spawn("job_wait "//_name//" "//_shot));
}
