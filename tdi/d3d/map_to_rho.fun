FUN PUBLIC MAP_TO_RHO(IN _signal, IN _efit, IN _shot)
{ 

  TCL('set tree D3D/shot='//adjustl(_shot));    
  _fullpath = getnci(_signal,"FULLPATH"); 
  _rank = rank(build_path(_fullpath));
  if ( _rank == 1) { 
     _dim0 = size(build_path(_fullpath),0);
     _data = zero([_dim0],1d0); 
  }
  if ( _rank == 2) {
     _dim0 = size(build_path(_fullpath),0);
     _dim1 = size(build_path(_fullpath),1);
     _data = zero([_dim0,_dim1]);  
  }

  _dim0s = trim(adjustl(_dim0//""));
  _dim1s = trim(adjustl(_dim1//""));
  _shots = trim(adjustl(_shot//""));

  _st = RhoMap->mapto(_signal,_efit,REF(_data),_dim0s,_dim1s,_shots);
  return(_data); 
}
