
PUBLIC FUN ECEPROF(OPTIONAL IN _shot, OPTIONAL IN _chanfirst, OPTIONAL IN _chanlast, OPTIONAL IN _skip16) {

/* Make 32 successive PTDATA calls to assemble the ECE temperature profile.
   Created: 1999.09.25  Jeff Schachter 

   Rewrote to build a profile for a variable amount of channels, with a max of 40.
   Updated to use PTDATA2.  Added optional to skip channel 16. - SF 20041124 
*/

   IF (NOT PRESENT(_shot))      _shot      = $SHOT;
   IF (NOT PRESENT(_chanfirst)) _chanfirst = 1;
   IF (NOT PRESENT(_chanlast))  _chanlast  = 40;
   IF (NOT PRESENT(_skip16))    _skip16    = 0;
   _err = 0;

   _cmd    = 'PTDATA2("ECE'//TRIM(ADJUSTL(_chanfirst))//'",_shot)';
   _profile = EXECUTE(_cmd); 
   _t      = DIM_OF(_profile);
   _dim    = _chanfirst;
   for (_i=_chanfirst+1; _i<=_chanlast; _i++) {
      if ( eq(_skip16,1) && eq(_i,16) ) { ; /* skip channel 16 */ }     
      else { 
         _cmd     = 'PTDATA2("ECE'//TRIM(ADJUSTL(_i))//'",_shot,1,_err)'; 
         _channel = EXECUTE(_cmd); 
         if (NE(_err,0) && NE(_err,2) && NE(_err,4)) { break; }
         _profile = [_profile,_channel];
         _dim    = [_dim,_i];   
      }
   }
   _profile = 1000. * _profile;

   return(MAKE_SIGNAL(MAKE_WITH_UNITS(_profile,"eV"),,_t,MAKE_WITH_UNITS(_dim,"channel")));

}

