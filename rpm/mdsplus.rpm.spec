Name: mdsplus
Version: 1.0
Release: 1
Copyright: GNU GPL
Group: Applications/Acquisition
Source: mdsplus-1.0.tar.gz
Buildroot: /var/tmp/mdsplus
Vendor: MIT Plasma Science and Fusion Center 
Summary: MDSplus Data Acquisition System

%ifos Linux
AutoReqProv: no
requires: openmotif
%endif

%description
Main libraries and routines to get MDS operational

%prep
echo " in prep"
%setup
echo " in setup"
%build
echo " in build"
./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --enable-nodebug
make

%install
echo " in install"
rm -rf $RPM_BUILD_ROOT
make install

%clean
echo " in clean"
#mv $RPM_BUILD_ROOT/usr/local/matlab /root/mattmp
rm -rf $RPM_BUILD_ROOT

%post 
/usr/local/mdsplus/rpm/post_install_script

%postun
/usr/local/mdsplus_post_uninstall_script

%files
%defattr(-,root,root)
/usr/local/mdsplus
%doc README README.INSTALL

%changelog
* Wed Aug 29 2000 Basil P. DUVAL <basil.duval@epfl.ch>
- version 1.0
