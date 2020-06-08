/* DevClearLastError(IN _expt, IN _shot, IN _action_path)
 * RETURNS DTYPE_T
 *
 * Used by jServer to examine details about failed Actions.
 * c.f. DevGetLastError
 */
PUBLIC FUN DevClearLastError(IN _expt, IN _shot, IN _action_path)
{
  PUBLIC _last_device_error=*;
  RETURN(1);
}
