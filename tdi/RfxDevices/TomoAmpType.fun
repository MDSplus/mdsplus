public fun TomoAmpType(in _id)
{
   return( LONG( (_id >> 16) & 0x1FQU ) );
}
