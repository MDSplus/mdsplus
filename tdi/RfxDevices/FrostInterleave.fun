public fun FrostInterleave(in _array1, in _array2, in _row)
{
/* Interleaves _array1 and _array2 and return as a 1024 x (2* size(_array1)) vector */
	_len = size(_array1);

	_a = replicate([0, _len], 0, _len);
	_b = long(0:(_len - 0.5):0.5);
	_idxs = _a + _b;


	_comp = [_array1, _array2, []];

	_interleaved = _comp[_idxs];

	_z = zero([_row, _len/_row]);

	return(_z + _interleaved);
}
