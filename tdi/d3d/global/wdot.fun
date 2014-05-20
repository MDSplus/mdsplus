/* wdot(in _efit, optional in _width) 
   This function calculates wdot and wplasma for a given efit.  Width is
   an optional argument for smoothing.  Default width is set to 0.5 if not
   provided - requires physicist input, as 0.5 is an educated guess taken
   from wdot.pro. An MDSplus tree must be open at the time of call (most 
   likely, transport).  
   20080829 SMF
*/  

public fun wdot(in _efit, optional in _width) {

   private fun spline(in _x, in _y, in _nx){
      return(CSVAL(_nx,CSINT(MAKE_SIGNAL(_y,*,_x)))); /*cubic spline*/
   };

   private fun igood(in _y, in _wdia, in _bpolav, in _ip, in _time, in _etime){
      return( PACK( _y, _wdia > 0. and 
                        ABS(_bpolav) > 0. and 
                        _ip > 0. and _time > 0. and 
                        _time <= MAXVAL(_etime) 
                  ) 
            );
   };

   /* Call getdia FORTRAN shared library */
   _phidia = getdia($SHOT,_time,_phidia,_sigphi,0.);
  
   /* Retrieve EFIT data */
   _etime  = DATA(USING_SIGNAL('\\atime',_efit)) / 1000.;  
   _volume = DATA(USING_SIGNAL('\\volume',_efit));
   _volume = spline(_etime,_volume,_time);    
   _s1     = DATA(USING_SIGNAL('\\S1',_efit));
   _s1     = spline(_etime,_s1,_time);
   _s2     = DATA(USING_SIGNAL('\\S2',_efit));
   _s2     = spline(_etime,_s2,_time);
   _ip     = DATA(USING_SIGNAL('\\IPMEAS',_efit));
   _ip     = spline(_etime,ABS(DATA(_ip)),_time);
   _area   = DATA(USING_SIGNAL('\\AREA',_efit)); 
   _area   = spline(_etime,_area,_time);
   _bpolav = DATA(USING_SIGNAL('\\BPOLAV',_efit));  
   _bpolav = spline(_etime,_bpolav,_time);
   _bcentr = DATA(USING_SIGNAL('\\BCENTR',_efit));
   _bcentr = spline(_etime,_bcentr,_time);
   _rbcent = DATA(USING_SIGNAL('\\RBCENT',_efit));
   _rbcent = spline(_etime,_rbcent,_time);
   _wdia   = DATA(USING_SIGNAL('\\WDIA',_efit));
   _wdia   = spline(_etime,_wdia,_time);
   _wmhd   = DATA(USING_SIGNAL('\\WMHD',_efit));
 
   /* calculate diamag */
   _mu0 = 4.*$PI*1.e-7;
   _c2c    = _bpolav^2 * _volume / 4.e-3 / $PI / _bcentr / _rbcent;
   _rcurrm = _volume / _area / 2. / $PI;
   _betap  = 2. * _wdia / 3. / _volume / (_bpolav^2/2./_mu0);
   _diamag = _c2c * (_s1/2. + _s2/2. * (1. - _rcurrm/_rbcent) - _betap);

   /* call igood for diamag / phidia */
   _ig_diamag = igood(_diamag, _wdia, _bpolav, _ip, _time, _etime);
   _ig_phidia = igood(_phidia, _wdia, _bpolav, _ip, _time, _etime);
   _sumx      = SUM(_ig_diamag); 
   _sumxx     = SUM(_ig_diamag^2); 
   _sumy      = SUM(_ig_phidia); 
   _sumxy     = SUM(_ig_diamag * _ig_phidia);
   _ngood     = SIZE(_ig_diamag);
  
   /* calculate det / c10 / c11 */ 
   _det = _ngood * _sumxx - _sumx^2;
   _c10 = (_sumy * _sumxx - _sumxy * _sumx) / _det;
   if ( GT(ABS(_c10),ABS(MINVAL(_ig_diamag))/10.) ) {_c10 = 0.;}
   _c11 = (_ngood * _sumxy - _sumx * _sumy) / _det;
   if ( LT(_c11,0.9) || GT(_c11,1.1) ) {_c11 = 1.0;}

   /* calculate betap */
   _betap = -1. * ( (_phidia - _c10)/_c11/_c2c - (_s1/2. + _s1/2. * (1.-_rcurrm/_rbcent)) );
   if ( lt(_betap[0],0.1) ) {_temp=0.;} else {_temp=_betap[0];}
   for (_i=1;_i<SIZE(_betap);_i++) { 
      if ( lt(_betap[_i],0.1) ) {_temp = [_temp,0.];} else {_temp=[_temp,_betap[_i]];}
   }
   _betap = _temp; _temp = 0.;  

   /* calculate w */
   _dt = _time[1] - _time[0];
   IF (NOT PRESENT(_width)) _width=0.1;
   _w = 1.5 * _betap * _volume * (_bpolav^2/2./_mu0);
   if ( lt(_w[0],0.) ) {_temp = 0.;} else { _temp = _w[0]; }
   for (_i=1;_i<SIZE(_w);_i++) {
      if ( lt(_w[_i],0.) ) {_temp = [_temp,0.];} else { _temp = [_temp,_w[_i]]; }
   }
   _w = _temp; _temp = 0.;
   _sw = CSSMOOTH(_time,_width,CSAKM(MAKE_SIGNAL(_w,*,_time)));

   /* filter non-finite values */
   _sw   = igood(_sw,   _wdia, _bpolav, _ip, _time, _etime);
   _w    = igood(_w,    _wdia, _bpolav, _ip, _time, _etime);
   _time = igood(_time, _wdia, _bpolav, _ip, _time, _etime);
   
   /* calculate wdot / swdot */
   _wdot  = CSDER(_time,CSAKM(MAKE_SIGNAL(_w,*,_time)));
   _swdot = CSDER(_time,CSAKM(MAKE_SIGNAL(_sw,*,_time)));

   PUBLIC __wdot    = MAKE_SIGNAL(_swdot,*,_time*1000.);
   PUBLIC __wplasma = MAKE_SIGNAL(_w,*,_time*1000.); 
   
   return(__wdot);
}
