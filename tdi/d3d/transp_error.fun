public fun transp_error(in _run_name, in _completion_status)
{

  /* Some debug for the logfile... */
  write(*, "\ntransp_error.fun was called\n");
  write(*, "run_name = " // _run_name // "\n");
  write(*, "completion_status = " // _completion_status // "\n");

  /* Figure out shot number */
  _shot = remove_run_string(_run_name);

  /* Tell DAM that we have error (192 = error)*/
  damphase(192, _shot);

  /* Return 0 to indicate good status */
  return (0);
}

