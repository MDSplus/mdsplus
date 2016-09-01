FUN PUBLIC ALIGN(IN _s1,IN _s2, OPTIONAL _subscript) {
/*
    Subscript two signals using the same x axis and return
    a signal with the signal being _s1[_x] and the dimension
    begin _s2[_x];
*/
	_newx = dim_of(present(_subscript) ? _s1[_subscript] : _s1);
        _newx = dim_of(_s2[_newx]);
        return(make_signal(data(_s1[_newx]),*,data(_s2[_newx])));
}

