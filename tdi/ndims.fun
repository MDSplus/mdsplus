public fun ndims(in _sigxxx) {
/* ndims(signal) returns number of dimensions defined in signal structure
   ndims(array) returns rank of array
  ndims(scalar) returns 0
*/
for (_n=7;_n>-1 && if_error((dim_of(_sigxxx,_n),1),0) == 0; _n--);
return(_n+1);
}
