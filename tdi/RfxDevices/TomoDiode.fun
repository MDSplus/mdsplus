public fun TomoDiode(in _id)
{
   return(  LONG( (QUADWORD(_id) >> 7) ) & 0x1FF );
}
