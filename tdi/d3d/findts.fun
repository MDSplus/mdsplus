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
/		<suffix>    is: ###, M##       
/                           or: L<string>    
/         and
/               <string>    is: a variable length string of the TS channel label
/		
/	Regarding the possible suffixes:
/
/		###, M##: Will return the data of the channel of which z (cm) position is closest to the argument.  
/			  The 'M' means negative z position.
/			  For example... TSTE_001 or TSTE_M06
/
/               L<string>: Will return the data of the channel that contains the TS label 
/                          specified by the input <string>; calls into tsslice_label.py.
/
/	Special Note:
/		
/		Tag names of the form TSTE_CORE, etc are not actually processed by this routine and the return
/	 	value of findts() will be 0 such that it will allow findsig() to search through the MDSplus
/		tree for those tags.  This is due to the fact that those are real tag names in the ELECTRONS
/		tree while the other possible syntaxes mentioned above are not real tag names and are being
/		translated into MDSplus TDI expresisons that call tsslice(), tsslice2(), os tsslice_label().
/
*/

PUBLIC FUN FINDTS (IN _tag, OPTIONAL OUT _tree, OPTIONAL OUT _node, OPTIONAL OUT _revert)
{
	/* Initialize returned values */
	_tree   = "";
	_node   = "";
	_revert = 0;
  
        /* Inititalize processing variables */
        _tag    = UPCASE(TRIM(_tag));
        _tssig  = TRIM(EXTRACT(0,4,_tag));
        _prefix = TRIM(EXTRACT(0,5,_tag));
        _suffix = TRIM(EXTRACT(5,LEN(_tag)-1,_tag));
        _suffix_fchar = EXTRACT(0,1,_suffix); 
        _flag = 0;    /*  0: use tsslice()   for argument z pos;      */
                      /*  1: use tsslice2()  for argument chan index  */

        /* Check that the tag name begins with "TSNE_" or "TSTE_" */
        if ( (_prefix ne "TSTE_") && (_prefix != "TSNE_") ) { return(0); }

        /* L<string> */
        if (_suffix_fchar eq "L") {
            _tree = 'ELECTRONS';
            _label = TRIM(EXTRACT(1,LEN(_tag)-1,_suffix));
            _node = 'TSSLICE_LABEL("'//_tssig//'",$SHOT,"'//_label//'")';
            return(1);
        }

        /* M## */ 
	if (_suffix_fchar eq "M") {
	  _suffix = EXTRACT(1,LEN(_suffix)-1,_suffix);
	}

        /* The _suffix has had the valid alpha char popped off. 
           Check that first char of the _suffix is a number? Why not check all chars? */
        _num=IACHAR(_suffix);
        _stat = ( (_num > 47) && (_num < 58) && (LEN(_suffix) >= 2) );
        if (_stat ne 1) { return(0); } 

        /* Attach a - char if the suffix was M## */
        if (_suffix_fchar eq "M") {
          _suffix = '-'//_suffix;
        }

        /* Craft the TSSLICE call */
        _tree = 'ELECTRONS';
        _revert = 1;
        if (_flag == 0) {
           _farg = FLOAT(EXECUTE(_suffix));
           if (_farg < -50.) {
              _system = 'DIV';
           } else {
              _system = 'CORE';
           }
           _tssig = '\\'//_tssig//'_'//_system;
           _node  = 'TSSLICE("'//_tssig//'",'//_suffix//',"'//_system//'")';
        } 

        /* Return */
	return (1);
	
}
