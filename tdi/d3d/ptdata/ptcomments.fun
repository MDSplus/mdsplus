FUN PUBLIC PTCOMMENTS(OPTIONAL IN _shot, OPTIONAL OUT _error) {

   private fun c(in _i,_idx){
      _idx = _idx + 8;
      return(char((_i[_idx/4]>>((_idx mod 4)*8)) & 0xff));
   };

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
        _pointname = 'COMMENTS  ';
        _file = ".PLA";
        _iarray = PTHEAD2_IFIX(_pointname, _shot);
        _npts = _iarray[31];
        _data = ZERO((_npts/2)+1, 0);

        _error  = 0;
        _rarray = ZERO(20, 0.0);
        _ascii  = [_iarray[35]/2.,ZERO(_iarray[35]/2.+1, 0)];
        _int16  = [_iarray[36],ZERO(_iarray[36]+1, 0)];
        _int32  = [_iarray[37]/2.,ZERO(_iarray[37]/2.+1, 0)];
        _real32 = [_iarray[38]/2.,ZERO(_iarray[38]/2.+1, 0.)];
        _iarray = [_npts,0,1,1,ZERO(46,0)];

        _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_", 
                             2, _shot, _file, _pointname, 
                             REF(_data), REF(_error), REF(_iarray), REF(_rarray), 
                             REF(_ascii), REF(_int16), REF(_int32), REF(_real32));  

           _text = "";
           _sec = 4*(_npts/4);

           if ( translatelogical("VENDOR") == "hp") {
              for ( _i=0; _i<_sec/4; _i++ ) {
                 _text = _text//c(_data,4*_i+3)//c(_data,4*_i+2)//c(_data,4*_i+1)//c(_data,4*_i);
              }        
           }
           else { for (_i=0; _i<_sec; _i++) { _text = _text//c(_data,_i); } }    

        return(_text);

}
