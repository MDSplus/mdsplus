public fun Tcl(in _command, optional out _output, optional out _error)
{
  _outp = present(_output);
  _errp = present(_error);
  _cmd = "set command tcl";
  if (_outp) {
    _output="";
    _error="";
    _status = Mdsdcl->mdsdcl_do_command_dsc(_cmd, xd(_error), xd(_output));
    if (_status & 1)
      _status =  Mdsdcl->mdsdcl_do_command_dsc(_command, xd(_error),  xd(_output));
    if (!_errp)
      _output=_output//_error;
  } else {
    _status = Mdsdcl->mdsdcl_do_command_dsc(_cmd, val(0), val(0));
    if (_status & 1)
      _status =  Mdsdcl->mdsdcl_do_command_dsc(_command, val(0),  val(0));
  }  
  return(_status);
}
