public fun PyDoMethod(as_is _nid, in _method,optional in _arg) {
   _n=getnci(_nid,'nid_number');
   if (present(_arg)) {
	 public __do_method_arg__=_arg;
   } else {
     public __do_method_arg__=*;
   }
   Py('Tree.doMethod('//text(_n)//',"'//_method//'")');
   return(public _result);
}
