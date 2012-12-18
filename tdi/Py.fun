public fun Py(in _cmd, optional in _varname) {
   fun PyCall(in _routine,_locked, optional in _arg) {
      if (_locked) {
        _GIL=build_call(51,getenv("PyLib"),"PyGILState_Ensure"); 
      }
      if (present(_arg)) {
        _ans=build_call(8,getenv("PyLib"),_routine,_arg);
      } else {
        _ans=build_call(8,getenv("PyLib"),_routine);
      }
      if (_locked) { 
        build_call(51,getenv("PyLib"),"PyGILState_Release",val(_GIL));
        if (ALLOCATED(public _PyReleaseThreadLock)) {
		build_call(8,getenv("PyLib"),"PyEval_ReleaseThread",val(build_call(51,getenv("PyLib"),"PyGILState_GetThisThreadState")));
		deallocate(public _PyReleaseThreadLock);
	}
      }
      return(_ans);
   }

   if (NOT ALLOCATED(public _PyInit)) {
     _sym=0qu;
     if (getenv("PyLib") == "") {
       write(*,"\n\nYou cannot use the Py function until you defined the PyLib environment variable!\n\n");
       write(*,"Please define PyLib to be the name of your python library, i.e. 'python2.4'\n\n\n");
       abort();
     }
     if (MdsShr->LibFindImageSymbol(descr('MdsMisc'),descr('PyCall'),ref(_sym)) == 1) {
       MdsMisc->PyCall("from MDSplus import Tree as ___TDI___Tree,execPy as ___TDI___execPy",val(1));
       public _PyInit=2;
     } else {
       if (MdsShr->LibFindImageSymbol(descr('dl'),descr('dlopen'),ref(_sym)) == 1) {
	  dl->dlopen('lib'//getenv("PyLib")//'.so',val(258));
       }
       PyCall("Py_Initialize",0);
       PyCall("PyEval_InitThreads",0);
       public _PyInit=1;
       PyCall("PyRun_SimpleString",1,"from MDSplus import Tree as ___TDI___Tree,execPy as ___TDI___execPy");
     }
   }
   public ___TDI___cmds=_cmd;
   if (present(_varname))
	_execCall="___TDI___execPy('"//_varname//"')";
    else
        _execCall="___TDI___execPy()";
   _locked=!present(_nolock);
   if (public _PyInit==1) {
     PyCall("PyRun_SimpleString",_locked,_execCall);
   } else {
     MdsMisc->PyCall(_execCall,val(_locked));
     if (ALLOCATED(public _PyReleaseThreadLock)) {
       MdsMisc->PyReleaseThreadLock();
       deallocate(public _PyReleaseThreadLock);
     }
   }
   if (allocated(public ___TDI___exception)) {
     public _py_exception=public ___TDI___exception;
     return(0);
   } else {
     public _py_exception="";
     return(public ___TDI___answer);
   }
}

