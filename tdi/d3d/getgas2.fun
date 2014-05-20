FUN PUBLIC GETGAS2(OPTIONAL IN _shot) {
	
	/* Default to $SHOT if not provided */
	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	/* Call ptcomments() to get the COMMENTS pointname.  Return if empty. */
	_comments = PTCOMMENTS(_shot);
	IF (EQ(_comments,"")) return(_comments);

	_gasa = TRIM(ADJUSTL( ELEMENT(2,'$',_comments) ));
	_gasb = TRIM(ADJUSTL( ELEMENT(3,'$',_comments) ));
	_gasc = TRIM(ADJUSTL( ELEMENT(4,'$',_comments) ));
	_gasd = TRIM(ADJUSTL( ELEMENT(5,'$',_comments) ));
	_gase = TRIM(ADJUSTL( ELEMENT(6,'$',_comments) ));

write (*,_comments);
write (*,'');
write (*,_gasa);
write (*,_gasb);
write (*,_gasc);
write (*,_gasd);
write (*,_gase);

	return([_gasa,_gasb,_gasc,_gasd,_gase]);


	/*_gasE = EXTRACT(0,2,ELEMENT(1,"=",ELEMENT(6,"$",_comments)));
	if (EXTRACT(0,1,_gasE) eq "-") 
	{
	    _gasE = "";
	}

	return([TRIM(ADJUSTL(ELEMENT(2,"$",_comments))), 
	        TRIM(ADJUSTL(ELEMENT(3,"$",_comments))), 
	        TRIM(ADJUSTL(ELEMENT(4,"$",_comments))), 
	        TRIM(ADJUSTL(ELEMENT(5,"$",_comments))), 
	        _gasE]);
	*/	
	
}

