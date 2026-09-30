FUN PUBLIC PARSE_IDENS (IN _signal, IN _indecies, IN _chord) 
{

  /*  _signal is a 1D array stored as follows:
  
 [ data(c0,t0), data(c0,t1) ... data(c0,tm), data(c1,t0), ... data(c1,tn) ... ] 
  
      _indecies is an array containing the starting index in _signal of 
      each chord:

                              [ 0, m, m+n, ...]

      It should have one more element than the number of chords (see below).

      So the number of time points for chord _i is 

                       _indecies[_i+1] - _indecies[_i]

      For example, with three chords labelled a-d, _signal looks like:

       _signal = [a0,a1,a2,a3,a4,b0,b1,b2,c0,c1,c2,c3,c4,c5,d0,d1,d2,d3]

           	   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17

      Then _indecies should look like:

                          _indecies = [0,5,8,14,18]

      _chord is an interger from 1 to the number of *possible* chords.
      It is used to subscript the array in _indecies to determine how to
      subscript signal.

      If a chord doesn't have valid data, then it indecies should be
      the same as the next indecies.  For example, if _signal looks like:

       _signal = [a0,a1,a2,a3,a4,b0,b1,b2,c0,c1,c2,c3,c4,c5,e0,e1,e2,e3,f0,f1]

           	   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19

      Then _indecies should look like:

                          _indecies = [0,5,8,14,14,18,20]


      Both _signal and _indecies can be node references.  For example, 
      if the tree looks like:

        TOP
	  :SIGNAL
	  :INDECIES

      then this function can be called as

	_result = WADE(\TOP:SIGNAL, \TOP:INDECIES, n);

  */

    /* NOTE!  The input parameters are not being validated!  For proper
    behavior regardless of inputs, they should be checked to make sure 
    they are of the appropriate size, shape, and value. */

   /* generate an array from the starting point of chord _chord to its end.
      This is the equivalent of indgen(n)+m. */

    _dataindecies=DATA(_indecies);

    if (_dataindecies[_chord+1] gt _dataindecies[_chord]) {
      _range = _dataindecies[_chord] .. _dataindecies[_chord+1]-1;  

      _data = DATA(_signal)[_range];   /* subscript data */
      _time = DATA(DIM_OF(_signal))[_range];  /* subscript time dimension */
      _units = UNITS_OF(_signal);
      _units_time = UNITS_OF(DIM_OF(_signal));

      RETURN ( MAKE_SIGNAL( MAKE_WITH_UNITS(_data,_units) , , 
	 	          MAKE_WITH_UNITS(_time,_units_time) ) );
    } else {
	abort();
    }


}
