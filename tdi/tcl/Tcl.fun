public fun Tcl(in _command, optional out _output, optional out _error)
{
  _outp = present(_output);
  _errp = present(_error);
  _cmd = ["set command tcl",_command];
  if (_outp || _errp) {
    _output="";
    _error="";
    _append=0;
  }
  for (_line=0; _line < size(_cmd); _line++) {
    if (_outp) {
      _out="";
      _err="";
      _status =  Mdsdcl->mdsdcl_do_command_dsc(_cmd[_line], xd(_err),  xd(_out));
      if (!_errp)
      	 _out=_out//_err;
      if (_append == 0 || size(_command) == 1)
         _output=_out;
      else
         _output=[_output, _out];
      if (_append == 0 || size(_command) == 1)
         _error=_err;
      else
         _error=[_error,_err];
      _append=1;
    } else {
      _status = Mdsdcl->mdsdcl_do_command_dsc(_cmd[_line], val(0), val(0));
    }
  }
  return(_status);
}
