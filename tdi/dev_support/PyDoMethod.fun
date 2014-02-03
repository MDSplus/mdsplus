public fun PyDoMethod(as_is _nid, in _method,optional in _arg) {
   _n=getnci(_nid,'fullpath');
   public _result=2; 
   if (present(_arg)) {
	 public __do_method_arg__=_arg;
   } else {
     public __do_method_arg__=*;
   }
   _cmds=["from MDSplus import Tree",
          "t=Tree('"//$expt//"',"//text($shot)//")",
          "n=t.getNode('"//_n//"')",
          "t.doMethod(n.nid,'"//_method//"')"];
   write(*,_cmds);
   Py(_cmds);
   if (public _py_exception != "") {
     write(*, _py_exception);
     public _result=662480186;
   }
   return(public _result);
}
