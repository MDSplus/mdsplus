fun public MdsConnect( in _host) {
  return(mdsremote->MdsConnect(REF(_host//char(0b))));
}
