public fun Ccl(in _command, optional out _output)
{
  Mdsdcl->mdsdcl_do_command('set command ccl_commands/def_file="*.ccl"');
  if (present(_output))
  {
    tcl_commands->TclSaveOut();
  }
  _status = Mdsdcl->mdsdcl_do_command(_command);
  if (present(_output))
  {
    _len = tcl_commands->TclOutLen();
    if (_len > 0)
    {
      _output = repeat(" ",_len);
      _len = tcl_commands->TclGetOut(val(1),val(_len),ref(_output));
    }
    else
      _output = "";
  }
  return(_status);
}
