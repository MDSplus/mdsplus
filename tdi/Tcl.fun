public fun Tcl(in _command)
{
  if (!allocated(_tcl$$initialized))
  {
    public _tcl$$initialized = Mdsdcl->mdsdcl_do_command('set command tcl');
  }
  return(Mdsdcl->mdsdcl_do_command(_command));
}
