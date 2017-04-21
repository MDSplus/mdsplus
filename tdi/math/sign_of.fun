/* Return the sign of the number (1, -1, or 0). */
FUN PUBLIC SIGN_OF(IN _number)
{
  RETURN( _number = 0 ? 0 : ( _number < 0 ? -1 : 1 ) );
}
