public fun TomoAmpId(in _id)
{
   return( LONG( (_id >> 21) & 0x7FQU ) );
}
