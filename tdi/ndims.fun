public fun ndims(in _sigxxx) {
/* ndims(signal) returns number of dimensions defined in signal structure
   ndims(array) returns rank of array
  ndims(scalar) returns 0
*/
for (_n=0;if_error(if_error(dim_of(_sigxxx,_n),'xyzzy') == 'xyzzy',0) == 0; _n++);
return(_n);
}
