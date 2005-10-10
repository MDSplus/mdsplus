/*
Name:  
	PTHEAD2

Purpose:  

	Retrieve all PTDATA headers for the requested PTDATA pointname

Calling Sequence:

	IARRAY = MDSVALUE (' PTHEAD2(  IN _pointname, 
                                       OPTIONAL IN      _shot, 
                                       OPTIONAL OUT     _error  ) ')
 
Input Parameters: 

	POINTNAME 	:  string	- pointname requested from PTDATA.
	SHOT		:  long		- shot number from which to retrieve PTDATA pointname.  
                   			  if not specified, defaults to current DIII-D shot
	
Outputs:

	IARRAY :  long - iarray header from the requested ptdata pointname
	ERROR  :  long - PTDATA error code

Procedure:

	PTHEAD2 is a TDI routine which retrieves all PTDATA headers for a given PTDATA pointname.  
	By default, iarray is automatically returned.  All headers retrieved from PTDATA are stored 
	as TDI public variables.  See below for additonal information.

Retrieving headers through PTDATA2 :

	To access the other PTDATA header arrays that PTHEAD2 stored in public TDI variables, add 
 	one additional expression to the MDSVALUE call.  

 		RARRAY = MDSVALUE (' PTHEAD2(...) , __RARRAY ')

	MDSplus returns the result of the last expression to be evalueated.  In the above example, 
	the PTHEAD2 routine is run, and the rarray header is requested as the returned variable.  
	It is helpful to note that while all TDI variables are preceeded by the underscore character, 
	TDI public variables are preceeded by a double underscore.  Public TDI varaibles are available 
	until they are either overwritten, or the current MDSPlus sesison is closed.  So, if retrieving 
	multiple header arrays for a single PTDATA pointname, it is better not to rerun the PTHEAD2 routine 
	each time.  

	Instead, do:   

		IARRAY  	= MDSVALUE (' PTHEAD2(...) ')
		RARRAY 		= MDSVALUE (' __RARRAY ')
		INT16 		= MDSVALUE (' __INT16 ')
		...	

Side Effects: 

	The ascii header is returned from PTDATA as an array of integer values.  This corresponds with 
	the previous implemntations of PTDATA header access (through gadat, for instance).  Please use 
        PTHEAD2_ASCII.fun for retrieval of the ascii header in string format. 

Required Software: MDSplus Client Software, PTDATA FORTRAN Library 

Author:  Sean Flanagan

Updated: 2005-10-10
*/
FUN PUBLIC PTHEAD2(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) {

        IF (NOT PRESENT(_shot)) _shot=$SHOT;
        _file = ".PLA";
        _iarray = PTHEAD2_IFIX(_pointname, _shot, 64, _error);
	
        _data   = 0;
        _error  = 0;
        _rarray = ZERO(20, 0.0);
        _ascii  = [_iarray[35]/2.,ZERO(_iarray[35]/2.+1, 0)];
        _int16  = [_iarray[36],ZERO(_iarray[36]+1, 0)];
        _int32  = [_iarray[37]/2.,ZERO(_iarray[37]/2.+1, 0)];
        _real32 = [_iarray[38]/2.,ZERO(_iarray[38]/2.+1, 0.)];
        _real64 = [_iarray[39]/4.,ZERO(_iarray[39]/4.+1, 0d0)];
        _time64 = ZERO(4,0.0);

        _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata64_", 
                             64, _shot, _file, _pointname//"          ", 
                             REF(_data), REF(_error), REF(_iarray), REF(_rarray), 
                             REF(_ascii), REF(_int16), REF(_int32), REF(_real32),
                             REF(_real64),REF(_time64));

        if (eq(_error,33)) { _error = 0; } /* Ignore error code 33. */

        PUBLIC __rarray = _rarray;
        PUBLIC __iarray = _iarray;
        PUBLIC __ascii  = _ascii;
        PUBLIC __int16  = _int16;
        PUBLIC __int32  = _int32;
        PUBLIC __real32 = _real32;
        PUBLIC __real64 = _real64;

        return(_iarray);

}
