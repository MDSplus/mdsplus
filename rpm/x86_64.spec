Name: mdsplus_x86_64
Version: #VERSION#
Release: #RELEASE#
Copyright: GNU GPL
Group: Applications/Acquisition
Source: mdsplus#GLOBUS#-#VERSION#.tar.gz
Prefix: /usr/local
Buildroot: /var/tmp/mdsplus
Vendor: http://www.mdsplus.org
Summary: MDSplus Data Acquisition System

AutoReqProv: no

%ifos Linux
requires: openmotif
%endif

%description
Main libraries and programs to get MDSplus operational

%prep

cvs -q -d :pserver:MDSguest:MDSguest@www.mdsplus.org:/mdsplus/repos co mdsplus

%build
cd mdsplus
JDKDIR=/opt/jdk1.5.0_08/
./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --enable-nodebug --with-jdk=$JDKDIR --target=i686-linux
make
mv bin bin32
./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --enable-nodebug --with-jdk=$JDKDIR
make clean
make
cd ..

%install
cd mdsplus
make install
mv $RPM_BUILD_ROOT/usr/local/mdsplus/bin $RPM_BUILD_ROOT/usr/local/mdsplus/bin64
cp -r bin32 $RPM_BUILD_ROOT/usr/local/mdsplus/
ln -sf bin64 $RPM_BUILD_ROOT/usr/local/mdsplus/bin
cp -r lib32 $RPM_BUILD_ROOT/usr/local/mdsplus/
ln -sf lib64 $RPM_BUILD_ROOT/usr/local/mdsplus/lib
cp -r uid32 $RPM_BUILD_ROOT/usr/local/mdsplus/
ln -sf uid64 $RPM_BUILD_ROOT/usr/local/mdsplus/uid

%clean
#rm -rf $RPM_BUILD_ROOT

%post 
$RPM_INSTALL_PREFIX/mdsplus/rpm/post_install_script

%postun

MDSPLUS_DIR=`cat /etc/.mdsplus_dir`
$MDSPLUS_DIR/local/mdsplus_post_uninstall_script

%files
%defattr(-,root,root)
/usr/local/mdsplus/bin
/usr/local/mdsplus/bin32
/usr/local/mdsplus/bin64
/usr/local/mdsplus/ChangeLog
/usr/local/mdsplus/etc
/usr/local/mdsplus/idl
/usr/local/mdsplus/include
/usr/local/mdsplus/java
/usr/local/mdsplus/LabView
/usr/local/mdsplus/lib
/usr/local/mdsplus/lib32
/usr/local/mdsplus/lib64
/usr/local/mdsplus/man
/usr/local/mdsplus/rpm
/usr/local/mdsplus/setup.csh
/usr/local/mdsplus/setup.sh
/usr/local/mdsplus/tdi
/usr/local/mdsplus/trees
/usr/local/mdsplus/uid
/usr/local/mdsplus/uid32
/usr/local/mdsplus/uid64
/usr/local/mdsplus/GLOBUS_LICENSE
/usr/local/mdsplus/libexec
/usr/local/mdsplus/sbin
/usr/local/mdsplus/setup
/usr/local/mdsplus/share
/usr/local/mdsplus/test
/usr/local/mdsplus/var
%dir /usr/local/mdsplus/local/tdi

%changelog
* Wed Aug 29 2000 Basil P. DUVAL <basil.duval@epfl.ch>
- version 1.0
