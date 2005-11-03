public fun XRAY_TREE(in _chord, optional _shot)
{
    _id_path = "\\DSXT::L_"//trim(adjustl(_chord))//":CHANNEL_ID";
    _chord_id = data(build_path(_id_path));
    
    
    _crate = (( _chord_id >> 8)&255) + 1;


    _pulse = "DSXTRAW"//trim(adjustl(_crate));
    
    if ( present(_shot))
	_pulseShot = _pulse//"/shot="//_shot;
    else
	_pulseShot = _pulse//"/shot="//$shot;



    tcl("set tree "//_pulseShot);



    _ch_path  = "\\DSXTRAW"//trim(adjustl(_crate))//"::RAW_"//trim(adjustl(_chord - 1));
       

       
    if_error( _sig = execute('build_path(`_ch_path)'), _status = 1, _status = 0);
    

    
    tcl("close  "//_pulseShot);



    return( _sig );


}
