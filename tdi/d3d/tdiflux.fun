FUN PUBLIC TDIFLUX(IN _shot, IN _time, IN _efit)
{ 
  _size = 0;
  _st = TdiFlux->sizeofdata(_shot,_time,_efit,"rho",REF(_size));  
  _data = (0:(_size-1):+1.0);
  WRITE(*,_data);
  _st = TdiFlux->mapto(_shot,_time,_efit,"rho",REF(_data));
  return(_data); 
}
