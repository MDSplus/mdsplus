
public fun L6810__INIT(as_is _nid, optional  _method)
{
  private fun ScaleCvt(in _scl) {
    _scls = [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4];
    for (_i=0; (_i<8) && (_scl>_scls[_i]); _i++);
    return(word(_i-1));
  }

  private fun FreqCvt(in _freq) {
     _freqs = [0, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 
               50000, 100000, 200000, 500000, 1000000, 2000000, 5000000];
     for (_i = 0; _i<size(_freqs) && _freq != _freqs[_i]; _i++) ;
     return(word(_i));
  }

  write (*, "Starting on init");  
  _c1_fs = DevNodeRef(_nid, 13);
  write (*, "got c1 fs "//_c1_fs);
  _scl = ScaleCvt(DevNodeRef(_nid, 13));
  write (*, "got scale "//_scl);
  _setup = [0w, 
            ScaleCvt(DevNodeRef(_nid, 13)),    /* sensitivity (0..3) */ 
            ScaleCvt(DevNodeRef(_nid, 19)),
            ScaleCvt(DevNodeRef(_nid, 25)),
            ScaleCvt(DevNodeRef(_nid, 31)),
            0w,                                /* block len */
            0w,                                /* num_blk_lsb */
            0w,                                /* num_blk_msb */
            1w,                                /* trigger hold off */
            0w,                                /* slope */
            0w,                                /* coupling */
            word(128+(2.4*128/5)),             /* trig up (2.5V) */
            255W,                              /* trig_lo */
            0W,                                /* trig source (external) */
            254W,                              /* near low */
            255W,                              /* near high */
            word(DevNodeRef(_nid, 9)),         /* active chans */
            word(DevNodeRef(_nid, 15)),        /* offset (0..3) */    
            word(DevNodeRef(_nid, 21)),        /* offset (0..3) */    
            word(DevNodeRef(_nid, 27)),        /* offset (0..3) */    
            word(DevNodeRef(_nid, 33)),        /* offset (0..3) */    
            word(DevNodeRef(_nid, 14)),        /* src coupling (0..3) */    
            word(DevNodeRef(_nid, 20)),        /* src coupling (0..3) */    
            word(DevNodeRef(_nid, 26)),        /* src coupling (0..3) */    
            word(DevNodeRef(_nid, 32)),        /* src coupling (0..3) */    
            0W,                                /* trigger delay 0 (all post trig) */
            word(LOG2(DevNodeRef(_nid, 8))),   /* samps per seg */
            word(DevNodeRef(_nid, 7) mod 256), /* num seg low */
            word(DevNodeRef(_nid, 7) / 256),   /* num seg high */
            0W,                                /* dual timebase */
            FreqCvt(DevNodeRef(_nid, 3)),      /* f1 */
            12W,                               /* f2 not used */
            word(DevNodeRef(_nid, 6))];
  write (*, "Have setup");
  write (*, _setup);
  show_private();
  _name = DevNodeRef(_nid,1);
  _status = DevCamChk(_name,CamPiow(_name,1,25,_d=0,16),1,*);
  write("did one camac i/o");
  _status = DevCamChk(_name,CamPiow(_name,0,2,_dummy,16),1,*);
  write (*, "done 2 i/os about to wait");
  l6810_wait(_name);
  write (*, "done wait");
  _status = DevCamChk(_name,CamPiow(_name,0,18,_dummy,16),1,*);
  write (*, "about to do Stopw");
  _status = DevCamChk(_name,CamStopw(_name,1,19,33,_setup,16),1,*);
  write(*, 'wrote setup');
  _status = DevCamChk(_name,CamPiow(_name,0,26,_dummy,16),1,*);
  _status = DevCamChk(_name,CamPiow(_name,6,18,_dummy,16),1,*);
  l6810_wait(_name);
  _status = DevCamChk(_name,CamPiow(_name,0,9,_dummy,16),1,*);
  return(1);
}

