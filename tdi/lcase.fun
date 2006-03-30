/* 
 *  Function lcase - convert a string from mixed case to all
 *       lower case.
 */
public fun lcase(in _str) {
  return (translate(_str, 
                   'abcdefghijklmnopqrstuvwxyz',
                   'ABCDEFGHIJKLMNOPQRSTUVWXYZ' ));
  
}
