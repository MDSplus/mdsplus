public fun TR10Readchannel(in _handle, in _chan, in _start, in _end, in _filter)
{
  _length = (_end-_start+1+31)/32*32;
  write(*, 'the length is '//_length);
  _buffer=zero([_length], 0w);
  TR10Error(TR10->TR10_Mem_Read_DMA(val(_handle), val(_chan), val(_start), ref(_buffer), val(_length)));
  _inc = 1;
  if (len(_filter) > 0) {
    public _increment = execute(_filter);
  }
  return(_buffer);
}
