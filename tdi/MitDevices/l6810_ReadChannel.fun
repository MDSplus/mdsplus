  /* 
     routine to read 1 channel from the 6810
     note that CamQstopw calls a routine which knows how to do
     I/Os > 64K bytes
   */
  Public fun l6810_ReadChannel( in _name , in _chan, in _segs, in _samples_per_segment, in _drop, in _samples_to_read, 
                                  OUT _channel_data )
  {
   write (*, "in read channel");
   DevCamChk(_name,CamPiow(_name, 5, 16, _d = 0, 16), 1, *);
    DevCamChk(_name,CamPiow(_name, 6, 16, _d = 0, 16), 1, *);
    DevCamChk(_name,CamPiow(_name, 7, 16, _d = 0, 16), 1, *);
    _ans_expr = "";
    for (_seg=0; _seg<_segs; _seg++) {
      write (*, "trying to read "//_seg);
      DevCamChk(_name,CamPiow(_name, _chan+1, 18, _seg, 16), 1, *);
      l6810_wait(_name);
      for (_i=0; _i<_drop; _i++) {
        DevCamChk(_name,CamPiow(_name, 0, 2, _dummy, 16), 1, *);
      }
      _seg_var = '_seg_'//trim(adjustl(_seg));
      write (*, "Do the stop");
      _cmd = "DevCamChk(_name, CamQstopw(_name, 0, 2, _samples_per_segment,"//_seg_var//",16),1,*)";
      _status = execute(_cmd);
      write (*, "stop done now concat the expr");
      _status = execute("_ans_expr = _ans_expr" // _seg_var //",");
    }
    _a_expr = "_channel_data = [" // EXTRACT( 0, len(_ans_expr)-2, _ans_expr) // "]";
    write (*, "Read all set, now :"//_a_expr);
    _status = execute(_a_expr);
    
   return(1);
  }
 
