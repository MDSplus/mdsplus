FUN PUBLIC RESAMPLE(IN _s, OPTIONAL _start, OPTIONAL _end, IN _inc) {
  _xin = *;
  if (NOT (PRESENT(_start) && PRESENT(_END))) {
    _xin = DATA(DIM_OF(_s));
  }
  _x = MAKE_RANGE(PRESENT(_start) ? _start : MINVAL(_xin),PRESENT(_end) ? _end : MAXVAL(_xin), _inc);
  return(MdsMisc->Resample:DSC(XD(_s),XD(_x)));
}
