/* 
/   This function determines whether a pointname passed to FINDSIG is
/   a special EFIT pointname.  Special EFIT pointnames are identified
/   by trailing suffixes:
/
/       _JT   = EFIT01
/       _MSE  = EFIT02
/	_RT1  = EFITRT1
/	_RT2  = EFITRT2 
/	_02ER = EFIT02ER
/       _nn   = EFITnn
/
*/

PUBLIC FUN FINDEFIT (INOUT _tag, OUT _tree)
{

   /* Initialize return values */ 
   _tree = "";
   _p=INDEX(_tag,"_",1);
   _l=LEN(_tag);
   _stat = 0; 	

   /* Try to determine if the tag name is an EFIT signal */
   if (_p > -1) {

      /* Extract the possible runtag */
      if ( (_p == _l-3) || (_p == _l-4) ) {
         _exten = TRIM(EXTRACT(_p+1,_l,_tag));

	 /* If there was a runtag: e.g. _JT, try to figure out 
	    what EFIT tree it is referencing. */
	 SWITCH (_exten) {
	    /* Between shot JT EFIT */
	    CASE ("JT") {
               _tree = "EFIT01";
	       _tag = EXTRACT(0,_p,_tag);
	       _stat = 1;	
	       BREAK;
	    }
            /* Between shot MSE EFIT */
	    CASE ("MSE") { 
	       _tree = "EFIT02";
	       _tag = EXTRACT(0,_p,_tag);
	       _stat = 1;     
	       BREAK;
	    }
            /* Between shot real-time JT EFIT */
	    CASE ("RT1") {
	       _tree = "EFITRT1";
	       _tag = EXTRACT(0,_p,_tag);
	       _stat = 1;     
	       BREAK;
	    }
	    /* Between shot real-time MSE EFIT */
	    CASE ("RT2") {
	       _tree = "EFITRT2";
	       _tag = EXTRACT(0,_p,_tag);
	       _stat = 1;     
	       BREAK;	
	    }
	    /* Between shot MSE ER EFIT */
	    CASE ("ER") {
               _tree = "EFIT02ER";
               _tag = EXTRACT(0,_p,_tag);
               _stat = 1;
               BREAK;
            }
	    /* Check if extension is nn (e.g. 01) */
	    CASE DEFAULT {
	       _num=IACHAR(_exten);
	       if ((_num > 47) && (_num < 58) && (LEN(_exten)==2) ) {
                  _tree = "EFIT"//_exten;
	          _tag = EXTRACT(0,_p,_tag);
		  _stat = 1;  
	       } 
	    }
	 } 
      } 
   } 
   return (_stat);
}

