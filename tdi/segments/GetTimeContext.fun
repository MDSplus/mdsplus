public fun GetTimeContext(out _start, out _end, out _delta){
  _start=*;
  _end=*;
  _delta=*;
  return(TreeShr->TreeGetTimeContext(xd(_start),xd(_end),xd(_delta)));
}

    
