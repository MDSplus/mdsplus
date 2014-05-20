/* 
   Nearest neighbor search, using the included binary search function 
   20100709 SMF 
*/

PUBLIC FUN NNSEARCH(IN _val, IN _array) { 

   _nleft  = bsearch(_val,_array,1);
   _nright = bsearch(_val,_array,-1);

   _min = MINVAL( ABS( [_val-_nleft,_val-_nright] ) );
write (*,_min);
   if   (_min eq 0) { _nearest = _nleft;}
   else             { _nearest = _nright;}

   return(_nearest);

}
