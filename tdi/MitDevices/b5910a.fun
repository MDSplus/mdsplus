/*      B5910A$CHAN.FUN
        Return channel output of Bira 5910
        _channel is channel name

        Tom Fredian, 27-SEP-1993
*/
FUN PUBLIC B5910A$CHAN(AS_IS _channel_nid) {
        _nid = GETNCI(_channel_nid,"NID_NUMBER");
        _elt = GETNCI(_nid,"CONGLOMERATE_ELT");
        _head_nid = _nid - _elt + 1;
        _freq = IF_ERROR(GETNCI(_head_nid + 3,"RECORD"),GETNCI(_head_nid + 4,"RECORD"));
        _trigger = IF_ERROR(GETNCI(_head_nid + 5,"RECORD"),0.0);
        _samples = GETNCI(_head_nid + 6,"RECORD");
        _window = make_window(0, _samples - 1, _trigger);
        _dim = make_dim(_window,_freq);
        _xout = data(_dim);
        _yout = zero(size(_xout),0.0);
        if (GETNCI(_nid,"STATE") == 0bu)
          _y = if_error(GETNCI(_nid + 1,"RECORD"),[0.,0.]);
        else
          _y = [0.,0.];
        _x = cvt(if_error(DATA(DIM_OF(_y)),[minval(_xout),maxval(_xout)]),0.0);
        _y = cvt(DATA(_y),0.0);
        if (GETNCI(_nid + 1,"STATE") == 0bu)
          MIT$DEVICES->b5910a_LINFIT(size(_x),_x,_y,size(_xout),_xout,ref(_yout));
        else
          MIT$DEVICES->b5910a_SPLFIT(size(_x),_x,_y,size(_xout),_xout,ref(_yout));
        return(make_signal(_yout,*,_dim));
}
