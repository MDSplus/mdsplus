public fun Py(in _cmd, optional in _varname, optional in _global_namespace, optional in _lock) {
   /*** _lock argument is now obsolete. Included in routine for compatibility only ***/
   _status = MdsMisc->PyCall("from MDSplus import execPy as ___TDI___execPy");
   if (_status & 1) {
     public ___TDI___cmds=_cmd;
     if (present(_varname))
       	_execCall="___TDI___execPy('"//_varname//"')";
      else
        _execCall="___TDI___execPy()";
   
     deallocate(public ___TDI___exception);
     deallocate(public ___TDI___answer);
     public ___TDI___global_ns= present(_global_namespace) ? 1 : 0;
     _status = MdsMisc->PyCall(_execCall);
     if (_status & 1) {
       if (allocated(public ___TDI___exception)) {
         public _py_exception=public ___TDI___exception;
	 write(*,public ___TDI___exception);
         _status=0;
       } else {
         public _py_exception="";
         _ans=public ___TDI___answer;
       }
     }
   }
   if (_status & 1)
     return(_ans);
   else {
     return(*);
   }
}

