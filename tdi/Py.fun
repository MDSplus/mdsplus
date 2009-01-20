public fun Py(in _cmd) {
   fun PyCall(in _routine,optional in _arg) {
      if (present(_arg)) {
        return(build_call(8,getenv("PyLib"),_routine,_arg));
      } else {
        return(build_call(8,getenv("PyLib"),_routine));
      }
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
     PyCall("PyRun_SimpleString","from MDSplus import *");
     public _PyInit=1;
   }
   for (_i=0;_i<size(_cmd);_i++) {
       PyCall("PyRun_SimpleString",_cmd[_i]);
   }
   return(1);
}
