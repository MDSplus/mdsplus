public fun Tcl(in _command)
{
  if (!allocated(public _tcl$$initialized))
  {
    public _tcl$$initialized = Mdsdcl->mdsdcl_do_command('set command tcl_commands');
  }
  return(Mdsdcl->mdsdcl_do_command(_command));
}
