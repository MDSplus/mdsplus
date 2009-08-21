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
epydoc -o $dir -v --name=MDSplus --docformat=epytext --no-private --css=grayscale apd.py connection.py event.py mdsarray.py compound.py ident.py mdsdata.py mdsdevice.py mdsscalar.py tree.py treenode.py
cd ..
rm -Rf /tmp/MDSplus

