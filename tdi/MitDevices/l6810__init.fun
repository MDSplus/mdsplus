public fun L6810__INIT(as_is _nid, optional  _method)
{
/* node offsets in the conglomerate */
private _K_CONG_NODES = 36;
Private _N_HEAD = 0;
Private _N_NAME = 1;
Private _N_COMMENT = 2;
Private _N_FREQ = 3;
Private _N_EXT_CLOCK_IN = 4;
Private _N_STOP_TRIG = 5;
Private _N_MEMORIES = 6;
Private _N_SEGMENTS = 7;
Private _N_ACTIVE_MEM = 8;
Private _N_ACTIVE_CHANS = 9;
Private _N_INPUT_01 = 10;
Private _N_INIT_ACTION = 34;
Private _N_STORE_ACTION = 35;

/* nid offsets for each channel */
Private _K_NODES_PER_CHANNEL = 6;
Private _N_CHANNELS = _N_INPUT_01;  
Private _N_CHAN_HEAD = 0;
Private _N_CHAN_STARTIDX = 1;
Private _N_CHAN_ENDIDX = 2;
Private _N_CHAN_GAINIDX = 3;
Private _N_CHAN_SRC_CPLING = 4;
Private _N_CHAN_OFFSET = 5;

  private fun ScaleCvt(in _scl) {
    _scls = [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4];
    for (_i=0; (_i<8) && (_scl>=_scls[_i]); _i++);
    return(word(_i-1));
  }

  private fun FreqCvt(in _freq) {
     _freqs = [0, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 
               50000, 100000, 200000, 500000, 1000000, 2000000, 5000000];
     for (_i = 0; _i<size(_freqs) && _freq != _freqs[_i]; _i++) ;
     return(word(_i));
  }

  _setup = [0w, 
            ScaleCvt(DevNodeRef(_nid, 13)),         /* sensitivity (0..3) */ 
            ScaleCvt(DevNodeRef(_nid, 19)),
            ScaleCvt(DevNodeRef(_nid, 25)),
            ScaleCvt(DevNodeRef(_nid, 31)),
            0w,                                      /* block len */
            0w,                                      /* num_blk_lsb */
            0w,                                      /* num_blk_msb */
            1w,                                      /* trigger hold off */
            0w,                                      /* slope */
            0w,                                      /* coupling */
            word(128+(2.4*128/5)),                   /* trig up (2.5V) */
            255W,                                    /* trig_lo */
            0W,                                      /* trig source (external) */
            254W,                                    /* near low */
            255W,                                    /* near high */
            word(DevNodeRef(_nid, _N_ACTIVE_CHANS)), /* active chans */
            word(DevNodeRef(_nid, 15)),              /* offset (0..3) */    
            word(DevNodeRef(_nid, 21)),              /* offset (0..3) */    
            word(DevNodeRef(_nid, 27)),              /* offset (0..3) */    
            word(DevNodeRef(_nid, 33)),              /* offset (0..3) */    
            word(DevNodeRef(_nid, 14)),              /* src coupling (0..3) */    
            word(DevNodeRef(_nid, 20)),              /* src coupling (0..3) */    
            word(DevNodeRef(_nid, 26)),              /* src coupling (0..3) */    
            word(DevNodeRef(_nid, 32)),              /* src coupling (0..3) */    
            0W,                                      /* trigger delay 0 (all post trig) */
            word(LOG2(DevNodeRef(_nid, _N_ACTIVE_MEM))), /* samps per seg */
            word(DevNodeRef(_nid, _N_SEGMENTS) mod 256), /* num seg low */
            word(DevNodeRef(_nid, _N_SEGMENTS) / 256),   /* num seg high */
            0W,                                          /* dual timebase */
            FreqCvt(DevNodeRef(_nid, _N_FREQ)),          /* f1 */
            12W,                                         /* f2 not used */
            word(DevNodeRef(_nid, 6))];
  _name = DevNodeRef(_nid,1);
  _status = DevCamChk(_name,CamPiow(_name,1,25,_d=0,16),1,*);
  _status = DevCamChk(_name,CamPiow(_name,0,2,_dummy,16),1,*);
  l6810_wait(_name);
  _status = DevCamChk(_name,CamPiow(_name,0,18,_dummy,16),1,*);
  _status = DevCamChk(_name,CamStopw(_name,1,19,33,_setup,16),1,*);
  _status = DevCamChk(_name,CamPiow(_name,0,26,_dummy,16),1,*);
  _status = DevCamChk(_name,CamPiow(_name,6,18,_dummy,16),1,*);
  l6810_wait(_name);
  _status = DevCamChk(_name,CamPiow(_name,0,9,_dummy,16),1,*);
  return(1);
}