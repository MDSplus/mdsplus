/*	MULTI.FUN
	A procedure to display multiple traces in a scope.

	Usage:

   MULTI(['trace-one-expression','trace-two-expression',...])
*/
FUN PUBLIC MULTI(IN _signals) {
        _y = *;
        _x = *;
        _n = size(_signals);
        for (_i=0; _i<_n; _i++) {
          _sig = execute(_signals[_i]);
          _y = [_y,$ROPRAND,_sig];
          _x = [_x,0.,dim_of(_sig)];
        }
        RETURN(make_signal(_y,*,_x));
}
