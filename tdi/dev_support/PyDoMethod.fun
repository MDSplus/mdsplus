public fun PyDoMethod(as_is _nid, in _method,optional in _arg) {
   public _result=2; 
   if (present(_arg)) {
	 public __do_method_arg__=_arg;
   } else {
     public __do_method_arg__=*;
   }
   _lock=index(upcase(_method),'SETUP') >= 0;
   Py(["from MDSplus import TreeRef",
          "TreeRef().doMethod("//text(getnci(_nid,"nid_number"))//",'"//_method//"')"],*,*,_lock);
   return(public _result);
}
