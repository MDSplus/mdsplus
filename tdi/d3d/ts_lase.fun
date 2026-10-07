PUBLIC FUN TS_LASE()
{

	/* Retrieve all three pieces of TS */
	_tc=IF_ERROR(DATA(\TSTIME_CORE),0);
	_td=IF_ERROR(DATA(\TSTIME_DIV),0);
	_tt=IF_ERROR(DATA(\TSTIME_TAN),0); 

        /* Initialize the parts of the mds string expression */
        _expr1 = '_ALLTIME = [';
        _expr2 = 'Make_Signal([';

        /* Check for TS CORE data and add to the expressions (if available) */
        IF (SIZE(_tc) GT 1) {
		_expr1=_expr1//'\\TSTIME_CORE';
                _expr2=_expr2//'\\TSLFOR_CORE';
        }

        /* Check for TS DIV data and add to the expressions (if available) */
        IF (SIZE(_td) GT 1) {
        	IF (EXTRACT(LEN(_expr1)-1,1,_expr1) NE "[") {
                	_expr1=_expr1//',';
                	_expr2=_expr2//',';
        	}
                _expr1=_expr1//'\\TSTIME_DIV';
                _expr2=_expr2//'\\TSLFOR_DIV';
        }

        /* Check for TS TAN data and add to the expressions (if available) */
        IF (SIZE(_tt) GT 1) {
        	IF (EXTRACT(LEN(_expr1)-1,1,_expr1) NE "[") {
                	_expr1=_expr1//',';
                	_expr2=_expr2//',';
        	}
                _expr1=_expr1//'\\TSTIME_TAN';
                _expr2=_expr2//'\\TSLFOR_TAN';
        }	

        /* Complete the mds string expression */
        _expr1  = _expr1//'], _ISORT = SORT(_ALLTIME), ';
        _expr2  = _expr2//'][_ISORT], *, Make_Dim(*, _ALLTIME[_ISORT]))';
        _ts_expr = _expr1//' '//_expr2;

        /* Execute the expression and return a signal object */
        _sig = EXECUTE(_ts_expr);
        RETURN(_sig);

}

