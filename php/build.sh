#!/bin/sh
cc -fpic -DCOMPILE_DL_MDSPLUS=1 -I/usr/local/include -I../../TSRM -I../.. -I../../main -I../../Zend -I. -I/home/mdsplus/mdsplus/include -c -o mdsplus.o mdsplus.c
 cc -shared -L/home/mdsplus/mdsplus/lib -rdynamic -o mdsplus.so mdsplus.o -lMdsIpShr
