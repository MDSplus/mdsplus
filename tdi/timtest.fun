public fun timtest(in _host)
{
  mdsconnect(_host);
  tcl('init timer');
  write(*,'Test of short transactions, 1000X4Bytes');
  for (_i=0;_i<1000;_i++) _x=mdsvalue('42');
  tcl('show timer');
  write(*,'Test of medium transactions, 1000X4000Bytes');
  for (_i=0;_i<1000;_i++) _x=mdsvalue('1:1000');
  tcl('show timer');
  write(*,'Test of large transactions, 100X1000000Bytes');
  for (_i=0;_i<100;_i++) _x=mdsvalue('1:250000');
  tcl('show timer');
  mdsdisconnect();
  return(1);
}
