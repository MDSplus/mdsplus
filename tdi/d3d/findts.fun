PUBLIC FUN FINDTS (IN _tag, OPTIONAL OUT _tree, 
	              OPTIONAL OUT _node, OPTIONAL OUT _revert)

/* This function determines whether a pointname passed to FINDSIG is 
   a special TS (Thomson Scattering) pointname.  TS pointnames are 
   identified as TSNE_nnn or TSTE_nnn.

   Created: 98.07.21 Jeff Schachter
*/

{
	/* initialize returned values */
	_tree = "";
	_node = "";
	_revert = 0;

	_l = LEN(_tag);

	_p = INDEX(_tag,"TSNE_");

	_stat = (_p == 0);

        if (not(_stat)) { 
	  _p = INDEX(_tag,"TSTE_");
	  _stat = (_p == 0);
	}


	if (_stat) { 
	  _exten = TRIM(EXTRACT(_p+5,_l,_tag));
	  if (UPCASE(EXTRACT(0,1,_exten)) eq "M") {
	    _exten = EXTRACT(1,LEN(_exten)-1,_exten);
	  }
	  _num=IACHAR(_exten);
	  _stat = ((_num > 47) && (_num < 58) && (LEN(_exten) >= 2) );
	} 

	if (_stat) {

	  _tree = "ELECTRONS";
	  _revert = 1;  /* GETALLDAT has logic (in GET_THOMSON) for handling TS file calls */

	  _type = EXTRACT(0,4,_tag);  /* either TSNE or TSTE */

	  _p = INDEX(_tag,"_");
	  _arg = TRIM(EXTRACT(_p+1,_l,_tag));

	  _c = EXTRACT(0,1,_arg);
	  if (_c == "M") {
	    _mult = -1.;
	    _arg = "-"//EXTRACT(1,LEN(_arg)-1,_arg);
	  } else { 
	    _mult = 1.;
	  }

	  _farg = FLOAT(EXECUTE(_arg));
	  if (_farg < -50.) {
	    _sys = "DIV";
	  } else { 
	    _sys = "CORE";
	  }

	  _sig =  "\\" // _type // "_" // _sys ;		

	  _node = 'TSSLICE("'//_sig//'",'//_arg//',"'//_sys//'")';


	}

	return (_stat);

}
