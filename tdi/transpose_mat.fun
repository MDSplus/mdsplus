/*	TRANSPOSE.FUN
	Transpose a 2D Matrix by constructing
        an array of subscripts and mapping the
        input array with it.

        Josh Stillerman 4/9/99
*/
PUBLIC FUN TRANSPOSE_MAT(IN _ARR)
{
  IF (RANK (_ARR) != 2) ABORT();
  _COLS = SIZE(_ARR, 0);
  _ROWS = SIZE(_ARR, 1);
  _SZ = _COLS*_ROWS;

  _IDX = (0 : _COLS-1) : _SZ-1 : _COLS;
  RETURN (SET_RANGE(_ROWS, _COLS, MAP(SET_RANGE(_SZ,_ARR), _IDX)));
}
