#!/bin/sh
pushd $(realpath $(dirname ${0})) >/dev/null 2>&1
rm -Rf doc
mkdir doc
core_py="apd.py version.py compound.py connection.py mdsarray.py mdsdata.py mdsdcl.py mdsExceptions.py mdsscalar.py tree.py"
docdir=$(pwd)/doc
tmpdir=$(mktemp -d)
mkdir ${tmpdir}/MDSplus
ln -s $(pwd)/*.py ${tmpdir}/MDSplus/
pushd ${tmpdir}/MDSplus >/dev/null 2>&1
epydoc -o $docdir -v --exclude='numpy'--name=MDSplus --docformat=epytext --no-private --css=grayscale ${core_py}
popd >/dev/null 2>&1
rm -Rf ${tmpdir}
popd >/dev/null 2>&1


