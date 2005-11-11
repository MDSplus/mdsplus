public fun transp_status(in _run_name, in _completion_status)
{

  /* Some debug for the logfile... */
  write(*, "\ntransp_status.fun was called\n");
  write(*, "run_name = " // _run_name // "\n");
  write(*, "completion_status = " // _completion_status // "\n");

  /* Figure out shot number */
  _shot = remove_run_string(_run_name);

  /* Tell DAM that we have completed (191=complete)*/
  damphase(191, _shot);

  /* Return 0 to indicate good status */
  return (0);
}

