#!/bin/sh
if [ -z "$1" ]
then
  dir="`pwd`/doc"
else
  dir="$1"
fi
mkdir MDSplus
cd MDSplus
ln -sf ../*.py .
epydoc -o $dir --name=MDSplus --docformat=epytext --css=grayscale apd.py array.py compound.py ident.py mdsdata.py scalar.py tree.py treenode.py
cd ..
rm -Rf MDSplus

