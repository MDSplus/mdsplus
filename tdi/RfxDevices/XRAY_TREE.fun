public fun XRAY_TREE(in _chord)
{
    _id_path = "\\DSXT::L_"//trim(adjustl(_chord))//":CHANNEL_ID";
    _chord_id = data(build_path(_id_path));
    
    _crate = (( _chord_id >> 8)&255) + 1;

    _ch_path  = "\\DSXTRAW"//trim(adjustl(_crate))//"::RAW_"//trim(adjustl(_chord - 1));
    
    return( execute('build_path(`_ch_path)') );
    
}
