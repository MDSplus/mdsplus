public fun echpwrc(in _shot) {

   _gyrotrons = ["BORIS","CHEWBACCA","HAN","KATYA","LEIA","LION","LUKE","NATASHA","SCARECROW","TINMAN"];
   _calibpath = "";
   _calibcopw = 0.0; 

   FOR (_i=0; _i<SIZE(_gyrotrons); _i++) {
      _calibpath       = BUILD_PATH("\\RF::TOP.ECH."//TRIM(_gyrotrons[_i])//":CALIBCOPW");
      _calibcopw_valid = GETNCI(_calibpath,"CLASS"); 
      if (ne(_calibcopw_valid,0)) { 
         _calibcopw = DATA(_calibpath);
         if (ne(_calibcopw,0)) { 
            _ecfpwr_path = BUILD_PATH("\\EC"//TRIM(EXTRACT(0,3,_gyrotrons[_i]))//"FPWRC"); 
            _ecfpwr = DATA(_ecfpwr_path);
            _ecftime = DIM_OF(_ecfpwr_path);
            IF (NOT PRESENT(_echpwrc)) { _echpwrc = _ecfpwr; 
                                         _echtime = _ecftime; } 
            ELSE { _echpwrc = _echpwrc + _ecfpwr; } 
         }
      }
   }

   _s = MAKE_SIGNAL(_echpwrc,*,_echtime);
   return(_s);

}

