
PUBLIC FUN FINDEFIT (INOUT _tag, OUT _tree)

/* This function determines whether a pointname passed to FINDSIG is 
   a special EFIT pointname.  Special EFIT pointnames are identified 
   by trailing suffixes:

	_JT   = EFIT01
	_MSE  = EFIT02
	_nn   = EFITnn

   Created: 98.07.21 Jeff Schachter

   Modification History:
     20030331 - added RT1 & RT2 cases - jrb & jrf
*/


{

	/* initialize return values */

	_tree = "";
	
	_p=INDEX(_tag,"_",1);
	_l=LEN(_tag);
	_stat = 0; /* initialize to return "Not an EFIT signal" */

	if (_p > -1) {
	  if ( (_p == _l-3) || (_p == _l-4) ) {
	    _exten = TRIM(EXTRACT(_p+1,_l,_tag));

	    SWITCH (_exten) {
	      CASE ("JT") {
		_tree = "EFIT01";
	        _tag = EXTRACT(0,_p,_tag);
		_stat = 1;	/* indicate that a special EFIT signal was found */
		BREAK;
	      }
	      CASE ("MSE") { 
		_tree = "EFIT02";
	        _tag = EXTRACT(0,_p,_tag);
		_stat = 1;     /* indicate that a special EFIT signal was found */
		BREAK;
	      }
	      CASE ("RT1") {
		_tree = "EFITRT1";
	        _tag = EXTRACT(0,_p,_tag);
		_stat = 1;     /* indicate that a special EFIT signal was found */
		BREAK;
	      }
	      CASE ("RT2") {
		_tree = "EFITRT2";
	        _tag = EXTRACT(0,_p,_tag);
		_stat = 1;     /* indicate that a special EFIT signal was found */
		BREAK;	
	      }
	      CASE DEFAULT {
		_num=IACHAR(_exten);
		if ((_num > 47) && (_num < 58) && (LEN(_exten)==2) ) {
		  _tree = "EFIT"//_exten;
	          _tag = EXTRACT(0,_p,_tag);
		  _stat = 1;  /* indicate that a special EFIT signal was found */
	        } 
	      }
	    } 
	  } 
	} 

	return (_stat);

}
