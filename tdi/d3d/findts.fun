/* 
/	This function determines whether a pointname passed to FINDSIG is a special TS (Thomson Scattering) pointname.  
/
/	There are multiple possible syntaxes of the form: 
/		
/		TS<quantity>_<suffix>
/
/	Where
/
/		<quantitiy> is: TE, NE 
/		<suffix>    is: CORE, TAN, DIV, ###, M##, C##, T##, D## 
/		
/	Regarding the possible suffixes:
/
/		CORE, TAN, DIV: Will return the multi-array data for that system's channels.
/				For example... TSTE_CORE
/
/		###, M##: Will return the data of the channel of which z (cm) position is closest to the argument.  
/			  The 'M' means negative z position.
/			  For example... TSTE_001 or TSTE_M06
/
/		C##, T##, D##: Will return the data of that system's channel index starting from 00.
/			       For example... TSTE_C00 -- TSTE_C40 or TSTE_D00 -- TSTE_D00
/
/	Special Note:
/		
/		Tag names of the form TSTE_CORE, etc are not actually processed by this routine and the return
/	 	value of findts() will be 0 such that it will allow findsig() to search through the MDSplus
/		tree for those tags.  This is due to the fact that those are real tag names in the ELECTRONS
/		tree while the other possible syntaxes mentioned above are not real tag names and are being
/		translated into MDSplus TDI expresisons via tsslice() and tsslice2().
/
*/

PUBLIC FUN FINDTS (IN _tag, OPTIONAL OUT _tree, OPTIONAL OUT _node, OPTIONAL OUT _revert)
{
	/* Initialize returned values */
	_tree = "";
	_node = "";
	_revert = 0;
        _flag = 0;    /*  0: use tsslice()   for argument z pos;      */
                      /*  1: use tsslicei2() for argument chan index  */


	/* Check that the tag name begins with "TSNE_" or "TSTE_" */
	_l = LEN(_tag);
	_p = INDEX(_tag,"TSNE_");
	_stat = (_p == 0);
        if (not(_stat)) { 
	  _p = INDEX(_tag,"TSTE_");
	  _stat = (_p == 0);
 	}

	/* Check that the suffix is one of the valid options */
	if (_stat) { 
	  _exten = TRIM(EXTRACT(_p+5,_l,_tag));
	  if (UPCASE(EXTRACT(0,1,_exten)) eq "M") {
	    _exten = EXTRACT(1,LEN(_exten)-1,_exten);
	  } else if (UPCASE(EXTRACT(0,1,_exten)) eq "C") {
	    _exten = EXTRACT(1,LEN(_exten)-1,_exten);
	    _flag = 1;
	  }
	  else if (UPCASE(EXTRACT(0,1,_exten)) eq "D") {
	    _exten = EXTRACT(1,LEN(_exten)-1,_exten);
	    _flag = 1;
	  }
	  else if (UPCASE(EXTRACT(0,1,_exten)) eq "T") {
	    _exten = EXTRACT(1,LEN(_exten)-1,_exten);
	    _flag = 1;
	  }
	  _num=IACHAR(_exten);
	  _stat = ((_num > 47) && (_num < 58) && (LEN(_exten) >= 2) );
	} 

	/* Begin processing the tag name and translate into the relevant MDSplus expression. */
	if (_stat) {

	  _tree = "ELECTRONS";
	  _revert = 1;  

	  /* Type is either: TSNE, TSTE */
	  _type = EXTRACT(0,4,_tag); 

	  /* Arg is the tag name suffix */
	  _p = INDEX(_tag,"_");
	  _arg = TRIM(EXTRACT(_p+1,_l,_tag));

	  /* Process additional settings based on the first character of the suffix */
	  _c = EXTRACT(0,1,_arg);
	  if (_c == "M") {
	    _mult = -1.;
	    _arg = "-"//EXTRACT(1,LEN(_arg)-1,_arg);
	  } else if (_c == "C" ) {
	    _arg = EXTRACT(1,LEN(_arg)-1,_arg);
            _sys = "CORE";
	  } else if (_c == "D" ) {
	    _arg = EXTRACT(1,LEN(_arg)-1,_arg);
            _sys = "DIV";
	  } else if (_c == "T" ) {
	    _arg = EXTRACT(1,LEN(_arg)-1,_arg);
            _sys = "TAN";
	  } else { 
	    _mult = 1.;
	  }

	  /* Based on the tag name syntax, call either tssslice() or tsslice2() */
          if (_flag == 0) {
  	    _farg = FLOAT(EXECUTE(_arg));
	    if (_farg < -50.) {
	      _sys = "DIV";
	    } else { 
	      _sys = "CORE";
	    }
            _sig =  "\\" // _type // "_" // _sys ;		
	    _node = 'TSSLICE("'//_sig//'",'//_arg//',"'//_sys//'")';
	  } else {
            _sig =  "\\" // _type // "_" // _sys ;		
	    _node = 'TSSLICE2("'//_sig//'",'//_arg//',"'//_sys//'")';
	  }

	}

	return (_stat);

}
