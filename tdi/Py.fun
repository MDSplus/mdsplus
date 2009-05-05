public fun Py(in _cmd) {
   fun PyCall(in _routine,optional in _arg) {
      if (allocated(public _PyInit)) 
        _GIL=build_call(51,getenv("PyLib"),"PyGILState_Ensure");
      if (present(_arg)) {
        _ans=build_call(8,getenv("PyLib"),_routine,_arg);
      } else {
        _ans=build_call(8,getenv("PyLib"),_routine);
      }
      if (allocated(public _PyInit)) 
        build_call(51,getenv("PyLib"),"PyGILState_Release",val(_GIL));
      return(_ans);
   }

   if (NOT ALLOCATED(public _PyInit)) {
     _sym=0qu;
     if (getenv("PyLib") == "") {
       write(*,"\n\nYou cannot use the Py function until you defined the PyLib environment variable!\n\n");
       write(*,"Please define PyLib to be the name of your python library, i.e. 'python2.4'\n\n\n");
       abort();
     }
     if (MdsShr->LibFindImageSymbol(descr('dl'),descr('dlopen'),ref(_sym)) == 1) {
	dl->dlopen('lib'//getenv("PyLib")//'.so',val(258));
     }
     PyCall("Py_Initialize");
     public _PyInit=1;
     PyCall("PyRun_SimpleString","from MDSplus import *");
   }
   for (_i=0;_i<size(_cmd);_i++) {
       public _py_exception="";
       PyCall("PyRun_SimpleString","try:\n    "//_cmd[_i]//"\nexcept Exception,exception:\n    String(exception).setTdiVar(\"_py_exception\")");
       if (public _py_exception != "") return(0);
   }
   return(1);
}
