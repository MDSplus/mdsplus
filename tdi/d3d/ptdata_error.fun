FUN PUBLIC PTDATA_ERROR(IN _error)
{
	if (_error == -1) {
		return("Size = 0");
	} else {
	 	_errmes="                                                                         ";
		_stat=libMdsD3D->mdspterror_(_error,REF(_errmes));
		return(_errmes);
	}
}
