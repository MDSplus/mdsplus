/*
 * Based on Read_Ufile.fun, written by TRG
 *
 * Read_UfScalars.fun
 *
 * Create: 27-Mar-98 PR
 *
 * Return EITHER the array of character information for all scalars in
 * the Ufile, OR the array of values of all the scalars, based on
 * final argument of call;
 * eventually,  arrays will be "remembered" between calls, so
 * that a call for the labels followed by a call for the values will not
 * result in two reads of the Ufile
 *
 */

public fun read_ufScalars(
    in   _ishot,
    in   _prefix,		/* 16 char limit */
    in   _ext,			/* 3 character file extension */
    in   _dev,			/* disk, no colon */
    in   _dir,			/* directory, no brackets */
    in   _req			/* request: LAB or lab or VAL or val */
   )

{
    _this$ufile = trim(_dev) // '_' // trim(_dir) //
                   '_' // trim(_prefix) //
                   '_' // trim(_ext) // '_' // trim(adjustl(_ishot));
    write(*,_this$ufile);

    _reqCaps = trim(upcase(_req));

    if (allocated(public _$ufile$name))
    {
        if (public _$ufile$name == _this$ufile)
        {
            write(*,"second call for same file...");
            if (_reqCaps=="VAL")
            {
                return( public _scvalues);
            }
            else if (_reqCaps=="LAB")
            {
                return( public _sclabels);
            }
            else
            {
                write(*,"Invalid request: "//_req);
                return(0);
            }
        }
        deallocate(public _$ufile$name);
        deallocate(public _$ufile$signal);
    }

    write(*,_reqCaps);
    if (_reqCaps!="LAB" && _reqCaps!="VAL")
    {
        write(*,"Invalid request code: "//_reqCaps);
        return(0);
    }

    _lun = 29;
    _sts = ufshr->ufsetr(_lun,_prefix,_ext,_dev,_dir);

    _ierr = 0;
    if (_ishot == 0)
    {
     _sts = ufshr->ufnsho(_lun);
    }

    _sts = ufshr->ufoprd(_lun,_ishot,ref(_ierr));
    if (_ierr != 0)
    {
        return(0);
    }

    _info = array(8,long(0));
    _sts = ufshr->ufhdck(_lun
                        ,ref(_info)
                        ,ref(_ierr));
    if (_ierr != 0)
    {
        _sts = ufshr->ufclos(_lun);
        return(0);
    }

    _ndim = _info[0];
    _npts = _info[1];		/* total num data pts in _fdat		*/
    _nx = _info[2];		/* size of 1st dim			*/
    _ny = _info[3];		/* size of 2nd dim (if any)		*/
    _nsc = _info[7];		/* number of scalars			*/

    if (_nsc > 0)
    {
        _scval = array(_nsc,0.0);	/*init	*/
        _sclab = array([3,_nsc],'          ');	/*init	*/
    }
    else
    {
        _scval = 0;			/*init		*/
        _sclab = " ";			/*init		*/
    }
    if (_ndim == 0)
    {				/* scalar data only		*/
        if (_nsc == 0)
        {				/*..oops			*/
            _sts = ufshr->ufclos(_lun);
            return(0);
        }
			/* read scalar data here ...			*/
        _sts = ufshr->uf0drd(_lun,descr(_tdev),descr(_sdate),
                             ref(_scval),descr( _sclab),
                               ref(_nsc), ref(_nscRet), ref(ierr));
        _sts = ufshr->ufclos(_lun);
        if (_ierr != 0)
        {
            return(0);
        }
        public _sclabels = _sclab;
        public _scvalues =  _scval;


         public _$ufile$name = _this$ufile;

        if (_reqCaps == "VAL")
        {
            return( _scval);
        }
        else
        {
            return( _sclab);
        }
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
    _nscRet = 0;			/*init				*/
    _xdat = array(_nx,0.0);		/*init				*/

    if (_ndim == 1)

    {			/*============= 1-dim ufile ============*/
        _fdat = array(_nx,0.0);		/*init	*/
        _sts = ufshr->uf1drd(_lun,descr(_tdev),descr(_sdate)
                        ,ref(_xdat),ref(_fdat),_nx,ref(_nxRet)
                        ,ref(_iproc),descr(_xlab),descr(_flab)
                        ,ref(_nsc),ref(_nscRet),ref(_scval),descr( _sclab)
                        ,ref(_ierr));
        write(*,'1-D ufile');
        if (_ierr != 0)
        {
            _sts = ufshr->ufclos(_lun);
            return(0);
        }
      public _sclabels = _sclab;
      public _scvalues = _scval;


    }
    else if (_ndim == 2)
    {			/*============= 2-dim ufile ============*/
        _ylab = _flab;			/*init				*/
        _ydat = array(_ny,0.0);		/*init				*/
        _fdat = array([_nx,_ny],0.0);	/*init		*/
        _iflag = 0;			/*init			*/
         _sts = ufshr->uf2drd(_lun,descr(_tdev),descr(_sdate)
                     ,ref(_fdat),_nx,ref(_xdat),_nx,ref(_ydat),_ny
                     ,ref(_iproc),ref(_nxRet),ref(_nyRet)
                     ,ref(_iflag),descr(_flab),descr(_xlab),descr(_ylab)
                     ,ref(_nsc),ref(_nscRet),ref(_scval),
                     descr( _sclab)
                     ,ref(_ierr));
        if (_ierr != 0)
        {
            _sts = ufshr->ufclos(_lun);
            return(0);
        }

       public _sclabels = _sclab;
       public _scvalues = _scval;


   }

    _sts = ufshr->ufclos(_lun);

    write(*,_scval);
    public _$ufile$name = _this$ufile;

    if (_reqCaps=="VAL")
    {
        return( _scval);
    }
    else
    {
        return( _sclab);
    }
}
