fun public MdsConnect( in _host) {
  return(MdsRemote->MdsConnect(REF(_host//char(0b))));
}
