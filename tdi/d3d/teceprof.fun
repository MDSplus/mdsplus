
PUBLIC FUN TECEPROF(OPTIONAL IN _shot, OPTIONAL IN _skip16) {

   /* Makes successive MDSplus calls to assemble the TECE temperature profile.
      Optional setting to skip channel 16 (a map-to-rho thing)   
      Created: 20041101 SF */

   IF (NOT PRESENT(_shot))   _shot   = $SHOT;
   IF (NOT PRESENT(_skip16)) _skip16 = 0;
   _stat=TreeShr->TreeOpen(ref("ECE\0"),val(_shot));
   _nchans = getnci(build_path("\\ECE::TOP.TECE"),"number_of_members");
   _nodes  = getnci(build_path(".TECE:*"),"fullpath");

   _profile = data(build_path(_nodes[0]));    
   for (_i=1; _i<_nchans; _i++) { 
      if ( eq(_skip16,1) and eq(_i,15) ) { ; /* skip channel 16 */ }
      else { _profile = [_profile,data(build_path(_nodes[_i]))]; }  
   }
   return(_profile);

}

