public fun cstring(in _ptr)
{
/* return string from pointer to cstring
********* NOTE THIS FUNCTION IS NON-PORTABLE AND *****
********* WILL NOT WORK ON PLATFORMS OTHER THAN  *****
********* 32 bit architectures                   *****
*/
   
  _ans=repeat(' ',32767);
  MdsShr->str_copy_dx(descr(_ans),val(_ptr));
  _ans=translate(_ans,"\1 "," \0");
  _ans=extract(0,index(_ans," "),_ans);
  return(translate(_ans," ","\1"));
} 
