/*      WOLFESUB.FUN
        Subscript a signal with a vector. Returns
        actual dimension values regardless of dimension type.
        Tom Fredian, 16-JUL-1995
*/
FUN PUBLIC WOLFESUB(IN _s,IN _subs) {
        return(make_signal(data(_s[_subs]),*,data(make_signal(data(dim_of(_s)),*,dim_of(_s))[_subs])));
}
