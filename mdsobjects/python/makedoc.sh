#!/bin/sh
thisdir=`pwd`
if [ -z "$1" ]
then
  dir="${thisdir}/doc"
else
  dir="$1"
fi
mkdir /tmp/MDSplus
cd /tmp/MDSplus
ln -sf ${thisdir}/*.py .
epydoc -o $dir -v --name=MDSplus --docformat=epytext --no-private --css=grayscale \
       apd.py \
       compound.py \
       connection.py \
       mdsarray.py \
       mdsdata.py \
       mdsdcl.py \
       mdsExceptions.py \
       mdsscalar.py \
       tree.py
rm -Rf /tmp/MDSplus

