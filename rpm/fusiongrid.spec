Name: fusiongrid
Version: 1.0
Release: 3
Copyright: GNU GPL
Group: Applications/Acquisition
Prefix: /usr/local
Buildroot: /tmp/fusiongrid
Vendor: www.fusiongrid.org
Summary: FusionGrid software including MDSplus and Globus

AutoReqProv: no

%ifos Linux
requires: openmotif
%endif

%description
Main libraries and programs to get MDSplus and Globus operational

%prep
cvs -q -d :pserver:anonymous@cvs.globus.org:/home/globdev/CVS/globus-packages co packaging
cvs -q -d :pserver:MDSguest:MDSguest@www.mdsplus.org:/mdsplus/repos co mdsplus

%build
cd packaging
./make-packages.pl --bundles=globus-gram,globus-gridftp --install=$RPM_BUILD_ROOT/usr/local/fusiongrid --deps --anonymous --flavor=gcc32
./make-packages.pl --packages=globus_xio --install=$RPM_BUILD_ROOT/usr/local/fusiongrid --deps --anonymous --flavor=gcc32pthr
cd ..
GLOBUS_LOCATION=$RPM_BUILD_ROOT/usr/local/fusiongrid
GPT_LOCATION=$RPM_BUILD_ROOT/usr/local/fusiongrid
export GLOBUS_LOCATION
export GPT_LOCATION
$GPT_LOCATION/sbin/gpt-build --disable-version-checking --installdir=$GLOBUS_LOCATION $MAKEKIT_DIR/myproxy*.tar.gz gcc32
$GPT_LOCATION/sbin/gpt-postinstall --force
LD_LIBRARY_PATH=$RPM_BUILD_ROOT/usr/local/fusiongrid/lib
export LD_LIBRARY_PATH
cd mdsplus
JDKDIR=/bigdisk/jdk/j2sdk1.4.2_04
./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/fusiongrid --enable-nodebug --with-xio=$RPM_BUILD_ROOT/usr/local/fusiongrid:gcc32 --with-jdk=$JDKDIR
find . -name makefile-header -exec rm -f {} \;
make
cd ..

%install
cp $MAKEKIT_DIR/fglogin $RPM_BUILD_ROOT/usr/local/fusiongrid/bin
cd mdsplus
make install
tar czf $RPM_BUILD_ROOT/usr/local/fusiongrid/setup/grid-security.tgz --exclude='*hostcert*' --exclude='*hostkey*' /etc/grid-security
%clean
#rm -rf $RPM_BUILD_ROOT

%post 
$RPM_INSTALL_PREFIX/fusiongrid/rpm/post_install_script

%postun

MDSPLUS_DIR=`cat /etc/.mdsplus_dir`
$MDSPLUS_DIR/local/mdsplus_post_uninstall_script

%files
%defattr(-,root,root)
/usr/local/fusiongrid/bin
/usr/local/fusiongrid/ChangeLog
/usr/local/fusiongrid/etc
/usr/local/fusiongrid/idl
/usr/local/fusiongrid/include
/usr/local/fusiongrid/java
/usr/local/fusiongrid/LabView
/usr/local/fusiongrid/lib
/usr/local/fusiongrid/man
/usr/local/fusiongrid/rpm
/usr/local/fusiongrid/setup.csh
/usr/local/fusiongrid/setup.sh
/usr/local/fusiongrid/tdi
/usr/local/fusiongrid/trees
/usr/local/fusiongrid/uid
/usr/local/fusiongrid/GLOBUS_LICENSE
/usr/local/fusiongrid/libexec
/usr/local/fusiongrid/sbin
/usr/local/fusiongrid/setup
/usr/local/fusiongrid/share
/usr/local/fusiongrid/test
/usr/local/fusiongrid/var
%dir /usr/local/fusiongrid/local/tdi

%changelog
* Wed Aug 29 2000 Basil P. DUVAL <basil.duval@epfl.ch>
- version 1.0
