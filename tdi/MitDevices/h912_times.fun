/*	H912_TIMES.FUN
	Return timestamps of H912 samples

        Builds a vector of dimensions, one for each block.

	Tom Fredian, 3-SEP-1997
*/
FUN PUBLIC H912_TIMES(IN _clock, IN _blocks, IN _blocksize, IN _trigger, IN _pts) {
        _ans = *;
        for (_i=0;_i < _blocks; _i++)
        {
          _win = build_window(_pts - _blocksize + 1, _pts, _trigger[_i]);
          _ans = vector(_ans, build_dim(_win,_clock));
        };
        return(make_with_units(set_range(size(_ans),_ans),"seconds"));
}
