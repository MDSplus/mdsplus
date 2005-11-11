/*
 *  Create:     TRG
 *
 *  02-Aug-01 CAL  Port to unix
 *
 *  09-Nov-99 CAL  units are 21:30 of _%lab
 *   1-Apr-98 PR   Make scalar labels and values public for later
 *                 possible use via READ_UFSCALARS.FUN
 *  27-Mar-98 PR   Increase length of _xlab, _flab
 *
 */
public fun read_ufile(
    in   _ishot
   ,in   _prefix
   ,in   _ext
   ,in   _dev
   ,in   _dir
   )
   {

    _this$ufile = trim(_dev) // '_' // trim(_dir) // '_' // trim(_prefix) //
             '_' // trim(_ext) // '_' // trim(adjustl(_ishot));
    if (allocated(public _$ufile$name))
       {
        if (public _$ufile$name == _this$ufile)
           {
	   if (allocated(public _$ufile$signal))
            return(public _$ufile$signal);
           }
        deallocate(public _$ufile$name);
        deallocate(public _$ufile$signal);
        deallocate(public _sclabels);
        deallocate(public _scvalues);
       }

    _lun = 29;
    _sts = ufshr->c_ufsetr(_lun,_prefix,_ext,_dev,_dir);
    if (_ishot == 0)
    {
     _sts = ufshr->ufnsho_(_lun);
    }

    _ierr = 0;				/*init	*/
    _sts = ufshr->ufoprd_(_lun,_ishot,ref(_ierr));
    write(*,'ufoprd_: ierr= ',_ierr);       
    if (_ierr != 0)
       {
        return(0);
       }

    _info = array(8,long(0));
    _sts = ufshr->ufhdck_(_lun
                        ,ref(_info)
                        ,ref(_ierr));
    if (_ierr != 0)
       {
        _sts = ufshr->ufclos_(_lun);
        return(0);
       }

    _ndim = _info[0];
    _npts = _info[1];		/* total num data pts in _fdat		*/
    _nx = _info[2];		/* size of 1st dim			*/
    _ny = _info[3];		/* size of 2nd dim (if any)		*/
    _nz = _info[4];		/* size of 3nd dim (if any)		*/
    _nsc = _info[7];		/* number of scalars			*/


    if (_ndim == 0)
       {				/* scalar data only		*/
        if (_nsc == 0)
           {				/*..oops			*/
            _sts = ufshr->ufclos_(_lun);
            return(0);
           }
			/* read scalar data here ...			*/
        _sts = ufshr->ufclos_(_lun);
        return(1);
       }
		/*======================================================
		 * Prepare variables for N-dimensional data ...
		 *=====================================================*/
    _tdev = "    ";			/*init: machine name (4 char)	*/
    _sdate = "          ";		/*init: shot date (10 char)	*/
    _flab = "                              ";	/* init: 30 char	*/
    _xlab = _flab;			/*init: ..likewise		*/
    _iproc = 0;				/*init				*/
    _nxRet = 0;				/*init				*/
    _nyRet = 0;				/*init				*/
    _nzRet = 0;				/*init				*/
    if (_nsc > 0)
       {
        _scval = array(_nsc,0.0);		/*init			*/
        _sclab = array([3,_nsc],'          ');	/*init			*/
       }
    else
       {
        /*
          _scval = 0;	
          _sclab = " ";
        */
        _scval = array(_nsc,0.0);		/*init			*/
        _sclab = array([3,_nsc],'          ');	/*init			*/
       }
    _nscRet = 0;			/*init				*/
    _xdat = array(_nx,0.0);		/*init				*/
    if (_ndim == 1)
       {			/*============= 1-dim ufile ============*/
        _fdat = array(_nx,0.0);		/*init	*/
/*
        _sts = ufshr->uf1drd_(_lun,descr(_tdev),descr(_sdate)
                        ,ref(_xdat),ref(_fdat),_nx,ref(_nxRet)
                        ,ref(_iproc),descr(_xlab),descr(_flab)
                        ,ref(_nsc),ref(_nscRet),ref(_scval),descr(_sclab)
                        ,ref(_ierr));
*/
        _sts = ufshr->c_uf1drd(_lun,_tdev,_sdate
                        ,ref(_xdat),ref(_fdat),_nx,ref(_nxRet)
                        ,ref(_iproc),_xlab,_flab
                        ,ref(_nsc),ref(_nscRet),ref(_scval),_sclab
                        ,ref(_ierr));

        if (_ierr != 0)
           {
            _sts = ufshr->ufclos_(_lun);
            return(0);
           }

	_funit = extract(20,10,_flab);
	_xunit = extract(20,10,_xlab);

        public _$ufile$signal = make_signal(
                    make_with_units(_fdat,_funit),
                    *,				/* RAW ???		*/
                    make_with_units(_xdat,_xunit));
       }
    else if (_ndim == 2)
       {			/*============= 2-dim ufile ============*/
        _ylab = _flab;			/*init				*/
        _ydat = array(_ny,0.0);		/*init				*/
        _fdat = array([_nx,_ny],0.0);		/*init	*/
        _iflag = 0;			/*init				*/
        _sts = ufshr->uf2drd_(_lun,descr(_tdev),descr(_sdate)
                    ,ref(_fdat),_nx,ref(_xdat),_nx,ref(_ydat),_ny
                    ,ref(_iproc),ref(_nxRet),ref(_nyRet)
                    ,ref(_iflag),descr(_flab),descr(_xlab),descr(_ylab)
                    ,ref(_nsc),ref(_nscRet),ref(_scval),descr(_sclab)
                    ,ref(_ierr));
        if (_ierr != 0)
           {
            _sts = ufshr->ufclos_(_lun);
            return(0);
           }

	_funit = extract(20,10,_flab);
	_xunit = extract(20,10,_xlab);
	_yunit = extract(20,10,_ylab);

        public _$ufile$signal = make_signal(
                    make_with_units(_fdat,_funit),
                    *,				/* RAW ???		*/
                    make_with_units(_xdat,_xunit),
                    make_with_units(_ydat,_yunit));
       }
    else if (_ndim == 3)
       {			/*============= 3-dim ufile ============*/
        _ylab = _flab;			/*init				*/
        _zlab = _flab;			/*init				*/
        _ydat = array(_ny,0.0);		/*init				*/
        _zdat = array(_nz,0.0);		/*init				*/
        _fdat = array([_nx,_ny,_nz],0.0);	/*init	*/
        _iflag = 0;			/*init				*/
        _sts = ufshr->uf3drd_(_lun,descr(_tdev),descr(_sdate)
                    ,ref(_fdat),_nx,_ny,ref(_xdat),_nx
                    ,ref(_ydat),_ny,ref(_zdat),_nz
                    ,ref(_iproc),ref(_nxRet),ref(_nyRet),ref(_nzRet)
                    ,ref(_iflag)
                    ,descr(_flab),descr(_xlab),descr(_ylab),descr(_zlab)
                    ,ref(_nsc),ref(_nscRet),ref(_scval),descr(_sclab)
                    ,ref(_ierr));
        if (_ierr != 0)
           {
            _sts = ufshr->ufclos_(_lun);
            return(0);
           }

	_funit = extract(20,10,_flab);
	_xunit = extract(20,10,_xlab);
	_yunit = extract(20,10,_ylab);
	_zunit = extract(20,10,_zlab);

        public _$ufile$signal = make_signal(
                    make_with_units(_fdat,_funit),
                    *,				/* RAW ???		*/
                    make_with_units(_xdat,_xunit),
                    make_with_units(_ydat,_yunit),
                    make_with_units(_zdat,_zunit));
       }

    _sts = ufshr->ufclos_(_lun);
    public _$ufile$name = _this$ufile;
    public _sclabels = _sclab;
    public _scvalues = _scval;
    return(public _$ufile$signal);
   }
