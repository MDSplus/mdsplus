public fun timtest(in _host)
{
  write(*,'Test of connect');
  tcl('init timer');
  mdsconnect(_host);
  tcl('show timer');
  tcl('init timer');
  write(*,'Test of short transactions, 1000x1Bytes');
  for (_i=0;_i<1000;_i++) _x=mdsvalue('42b');
  tcl('show timer');
  write(*,'Test of medium transactions, 1000X1000Bytes');
  tcl('init timer');
  for (_i=0;_i<1000;_i++) _x=mdsvalue('zero(1000,1b)');
  tcl('show timer');
  write(*,'Test of large transactions, 100X1000000Bytes');
  tcl('init timer');
  for (_i=0;_i<100;_i++) _x=mdsvalue('zero(1000000,1b)');
  tcl('show timer');
  mdsdisconnect();
  return(1);
}
