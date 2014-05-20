FUN PUBLIC GETGAS(OPTIONAL IN _shot)

{

	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	_comments = PTCOMMENTS(_shot);
	IF (EQ(_comments,"")) return(_comments);

	_gasE = EXTRACT(0,2,ELEMENT(1,"=",ELEMENT(6,"$",_comments)));
	if (EXTRACT(0,1,_gasE) eq "-") 
	{
	    _gasE = "";
	}

	return([TRIM(ADJUSTL(ELEMENT(2,"$",_comments))), 
	        TRIM(ADJUSTL(ELEMENT(3,"$",_comments))), 
	        TRIM(ADJUSTL(ELEMENT(4,"$",_comments))), 
	        TRIM(ADJUSTL(ELEMENT(5,"$",_comments))), 
	        _gasE]);
	
	
}

