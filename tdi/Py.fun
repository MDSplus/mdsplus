public fun Py(in _cmd, optional in _varname, optional in _global_namespace, optional in _lock) {
   if (NOT ALLOCATED(public _PyInit)) {
     _sym=0qu;
     if (getenv("PyLib") == "") {
       write(*,"\n\nYou cannot use the Py function until you defined the PyLib environment variable!\n\n");
       write(*,"Please define PyLib to be the name of your python library, i.e. 'python2.4'\n\n\n");
       abort();
     }
     MdsMisc->PyCall("from MDSplus import execPy as ___TDI___execPy",val(1));
   }
   public ___TDI___cmds=_cmd;
   if (present(_varname))
	_execCall="___TDI___execPy('"//_varname//"')";
    else
        _execCall="___TDI___execPy()";
   
   deallocate(public ___TDI___exception);
   deallocate(public ___TDI___answer);

   public ___TDI___global_ns= present(_global_namespace) ? 1 : 0;
   MdsMisc->PyCall(_execCall,val(1));
   if (allocated(public ___TDI___exception)) {
     public _py_exception=public ___TDI___exception;
     return(0);
   } else {
     public _py_exception="";
     return(public ___TDI___answer);
   }
}

