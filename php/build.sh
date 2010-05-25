#!/bin/sh
if [ -r /etc/.mdsplus_dir ]
then
  MDSPLUS_DIR="`cat /etc/.mdsplus_dir`"
else
  MDSPLUS_DIR="/usr/local/mdsplus"
fi
if [ -x /usr/bin/php-config ]
then
  PHP_INC="`/usr/bin/php-config --includes`"
  PHP_EXTDIR="`/usr/bin/php-config --extension-dir`"
else
  PHP_INCDIR="/usr/include/php"
  PHP_INC="-I${PHP_INCDIR} -I${PHP_INCDIR}/TSRM -I${PHP_INCDIR}/main -I${PHP_INCDIR}/Zend"
  PHP_EXTDIR="/usr/lib64/php/modules"
fi
cc -fpic -DCOMPILE_DL_MDSPLUS=1 ${PHP_INC} -I. -I${MDSPLUS_DIR}/include -shared -o ${PHP_EXTDIR}/mdsplus.so mdsplus.c -L${MDSPLUS_DIR}/lib -lMdsIpShr
