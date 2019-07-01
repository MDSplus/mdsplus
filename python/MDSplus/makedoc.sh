#!/bin/sh
set -e
pydir=$(realpath $(dirname ${0}))
cd $pydir
if test -d doc
then rm -Rf doc
fi
mkdir doc
core_py="apd.py version.py compound.py connection.py mdsarray.py mdsdata.py mdsdcl.py mdsExceptions.py mdsscalar.py tree.py"
docdir=$(pwd)/doc
tmpdir=$(mktemp -d)
mkdir ${tmpdir}/MDSplus
ln -s $(pwd)/*.py ${tmpdir}/MDSplus/
cd ${tmpdir}/MDSplus
epydoc -o $docdir -v --exclude='numpy'--name=MDSplus --docformat=epytext --no-private --css=grayscale ${core_py}
cd $pydir
rm -Rf ${tmpdir}
