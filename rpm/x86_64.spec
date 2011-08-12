Name: mdsplus
Version: %{?mdsver}
Release: %{?mdsrel}%{?dist}
License: GNU GPL
Group: Applications/Acquisition
Prefix: /usr/local
BuildRoot: /var/tmp/mdsplus
Vendor: http://www.mdsplus.org
Summary: MDSplus Data Acquisition System

AutoReqProv: yes

%description
Main libraries and programs to get MDSplus operational

%prep

cvs -q -d :pserver:MDSguest:MDSguest@www.mdsplus.org:/mdsplus/repos co mdsplus
chmod -R u+w mdsplus
%build
cd mdsplus
./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin32 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib32 --enable-nodebug --target=i686-linux --disable-java
make clean
make
./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin64 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib64 --enable-nodebug
make clean
make
cd ..

%install
cd mdsplus
make install
cp -r bin32 $RPM_BUILD_ROOT/usr/local/mdsplus/
cp -r lib32 $RPM_BUILD_ROOT/usr/local/mdsplus/
cp -r uid32 $RPM_BUILD_ROOT/usr/local/mdsplus/

%clean
rm -rf $RPM_BUILD_ROOT

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
/usr/local/mdsplus/mdsobjects
/usr/local/mdsplus/rpm
/usr/local/mdsplus/setup.csh
/usr/local/mdsplus/setup.sh
/usr/local/mdsplus/tdi
/usr/local/mdsplus/trees
/usr/local/mdsplus/uid
/usr/local/mdsplus/uid32
/usr/local/mdsplus/uid64
%dir /usr/local/mdsplus/local/tdi

%changelog
* Wed Aug 29 2000 Basil P. DUVAL <basil.duval@epfl.ch>
- version 1.0
