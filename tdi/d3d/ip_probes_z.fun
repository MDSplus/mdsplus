public fun ip_probes_z (in _shot) {
	
    	if ( ge(_shot,124400) ) {
		_y = ip_probes(_shot);
		_x = dim_of(ip_probes(_shot)); 

		_index = ge(_x,-50) && lt(_x,-40);
		_y0 = mean(pack(_y,_index));
	
		_vloopb = ptdata2("vloopb",_shot);
		_ip_probes_z = (_y - _y0) - (1490.*_vloopb);

		return(Make_Signal(_ip_probes_z,*,_x));
     	} else {
		return(0);
	}

}
