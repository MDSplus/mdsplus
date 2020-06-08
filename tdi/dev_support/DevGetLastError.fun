/* DevGetLastError(IN _expt, IN _shot, IN _action_path)
 * RETURNS DTYPE_T
 *
 * Used by jServer to examine details about failed Actions.
 * c.f. DevClearLastError 
 */
PUBLIC FUN DevGetLastError(IN _expt, IN _shot, IN _action_path)
{
  RETURN(IF_ERROR(TEXT(PUBLIC _last_device_error), ""));
}
