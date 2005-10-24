PUBLIC FUN FINDCER (IN _tag, OPTIONAL OUT _tree, 
	              OPTIONAL OUT _node, OPTIONAL OUT _revert)


/* This function determines whether a pointname passed to FINDSIG is 
   a special CER signal.  Special CER signals are the profiles:

	CERxabcsq

	where x = F, N, Q
	abc = TI, AMP, ROT
	s = nothing, T or V 
	q = nothing or A (for approximate)

   Created: 99.05.12 Jeff Schachter
*/

{
	/* initialize returned values */
	_tree = "";
	_node = "";
	_revert = 0;

	/* check to see if _tag starts with "CER" */

	_l = LEN(_tag);

	_p = INDEX(_tag,"CER");

	_stat = (_p == 0);

	IF (NOT(_stat)) 
	{
	  RETURN(_stat);
	}

	/* now check if _tag ends with a digit; if it does, this is
	 * not a special pointname
         */

	_num = IACHAR(TRIM(EXTRACT(_l-1,1,_tag)));

	_stat = ((_num < 47) || (_num > 58));
	IF (NOT(_stat))
	{
	  RETURN(_stat);
	}


	/* if pointname is a CER profile, compose _signal */

	_tree = "IONS";


	/* code (CERFIT, CERNEUR, CERQUICK) */
	_test = UPCASE(EXTRACT(3,1,_tag));
	SWITCH (_test)
	{
	  CASE ("F")  _code = "CERFIT"; BREAK;
	  CASE ("N")  _code = "CERNEUR"; BREAK;
	  CASE ("Q")  _code = "CERQUICK"; BREAK;
	  CASE DEFAULT  RETURN(0);
	}

	/* profile (TEMP, AMP, ROT) */
	_p=7;
	_test = UPCASE(EXTRACT(4,3,_tag));
	SWITCH (_test)
	{
	  CASE ("AMP")  
	  CASE ("ROT")  _prof = _test; BREAK;
	  CASE DEFAULT 
	  {
	     if (UPCASE(EXTRACT(4,2,_tag)) == "TI")
	     {
	        _prof = "TEMP";
	        _p = 6;
	     }
	     ELSE RETURN(0);
          }
	     
	}

	_node = 'CERPROF("'//_code//'","'//_prof//'"';

	/* system (nothing, tangential, vertical) */	

	_test = UPCASE(EXTRACT(_p,1,_tag));

	SWITCH (_test)
	{
	  CASE ("T")  _node = _node//',"TANGENTIAL"'; _p=_p+1; BREAK;
	  CASE ("V")  _node = _node//',"VERTICAL"'; _p=_p+1; BREAK;
	  CASE ("")   _node = _node//','; BREAK;
	  CASE ("A")  _node = _node//',,1)'; _p=_p+1; BREAK;
	  CASE DEFAULT   RETURN(0);
	}

	if (_test NE "A") 
        {
		
	  IF (UPCASE(EXTRACT(_p,1,_tag))  == "A") 
   	  {
	    _node = _node//',1)';
	    _ltest = _p+1;
	  }
	  ELSE
	  {
	    _node = _node//')';
	    _ltest = _p;
	  }
        }
	ELSE 
        {
	  _ltest = _p;
        } 

	RETURN(_ltest == _l);

}
