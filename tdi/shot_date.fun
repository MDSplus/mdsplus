/*	SHOT_DATE.FUN	*/
FUN	PUBLIC shot_date(OPTIONAL IN _shot, OPTIONAL IN _expt) {
	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	IF (NOT PRESENT(_expt)) _expt=$EXPT;
	IF (_shot<0) _shot='MODEL';
	ELSE IF (_shot==0) _shot=CURRENT_SHOT(_expt);
	_filename = _expt//'$DATA:'//_expt//'_'//TEXT(_shot)//'.TREE';
	return (file_date(_filename));
}
