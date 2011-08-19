Name: mdsplus
Version: %{?mdsver}
Release: %{?mdsrel}%{?dist}
License: GNU GPL
Summary: The MDSplus Data System
Group: Applications/Acquisition
Source: %{?kitname}.tar.gz
Prefix: /usr/local
BuildRoot: /var/tmp/mdsplus
Vendor: http://www.mdsplus.org
Summary: MDSplus Data Acquisition System
AutoReqProv: yes
requires: mdsplus-kernel
requires: mdsplus-java
requires: mdsplus-idl
requires: mdsplus-labview
requires: mdsplus-matlab
requires: mdsplus-motif
requires: mdsplus-devel
requires: mdsplus-camac
requires: mdsplus-mitdevices
requires: mdsplus-rfxdevices
requires: mdsplus-python
requires: mdsplus-mssql

%description
Main libraries and programs to get MDSplus operational

%package kernel
Summary: MDSplus core system
Group: Applications/Acquisition
%description kernel
Core applications, libraries and configuration files

%package java
Summary: Java Applications
Group: Applications/Acquisition
%description java
Java applications and classes

%package idl
Summary: ITT IDL extensions
Group: Applications/Acquisition
%description idl
IDL (ITT Interactive Data Language) extensions for MDSplus

%package labview
Summary: National Instruments Labview extensions
Group: Applications/Acquisition
%description labview
National Instruments Labview interface to MDSplus

%package matlab
Summary: Mathworks MATLAB extensions
Group: Applications/Acquisition
%description matlab
Mathworks MATLAB extensions

%package motif
Summary: X-Windows Motif based application
Group: Applications/Acquisition
%description motif
X-Windows applications which use the Motif window system. This package provides the following applications:
dwscope, dwpad, traverser, actions, actmon

%package camac
Summary: Support for CAMAC devices
Group: Applications/Acquisition
%description camac
Support for accessing CAMAC devices

%package mitdevices
Summary: Support for MIT data acquisition devices
Group: Applications/Acquisition
%description mitdevices
Support for MIT Data acquisition devices

%package rfxdevices
Summary: Support for RFX data acquisition devices
Group: Applications/Acquisition
%description rfxdevices
Support for RFX Data acquisition devices

%package python
Summary: Python interface to MDSplus
Group: Applications/Acquisition
requires: mdsplus-kernel,python,numpy,python-ctypes
#BuildArch: noarch
%description python
Python interface to MDSplus

%package mssql
Summary: Interface to mssql databases
Group: Applications/Acquisition
%description mssql
Interface to mssql databases

%package devel
Summary: Header files and static libraries for code development
Group: Applications/Acquisition
%description devel
Header files and static libraries for code development

%prep
%setup -q


%build
if [ "%_target" != "i686-linux" ]
then
  ./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin32 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib32 --enable-nodebug --target=i686-linux --disable-java
make clean
make
  ./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin64 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib64 --enable-nodebug
  make clean
  make
else
  ./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin32 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib32 --enable-nodebug --target=i686-linux
  make clean
  make
fi


%install

make install
if [ "%_target" |= "i686-linux" ]
then
  cp -r bin32 $RPM_BUILD_ROOT/usr/local/mdsplus/
  cp -r lib32 $RPM_BUILD_ROOT/usr/local/mdsplus/
  cp -r uid32 $RPM_BUILD_ROOT/usr/local/mdsplus/
else
  ln -sf bin32 $RPM_BUILD_ROOT/usr/local/mdsplus/bin
  ln -sf lib32 $RPM_BUILD_ROOT/usr/local/mdsplus/lib
fi
cp -r matlab $RPM_BUILD_ROOT/usr/local/mdsplus/

%clean
rm -rf $RPM_BUILD_ROOT

%post 
$RPM_INSTALL_PREFIX/mdsplus/rpm/post_install_script

%postun

MDSPLUS_DIR=`cat /etc/.mdsplus_dir`
$MDSPLUS_DIR/local/mdsplus_post_uninstall_script

%files

%files kernel
%defattr(-,root,root)
/usr/local/mdsplus/bin*
/usr/local/mdsplus/ChangeLog
/usr/local/mdsplus/etc
/usr/local/mdsplus/include
/usr/local/mdsplus/lib*
/usr/local/mdsplus/man
/usr/local/mdsplus/rpm
/usr/local/mdsplus/setup.csh
/usr/local/mdsplus/setup.sh
/usr/local/mdsplus/tdi
/usr/local/mdsplus/trees

%exclude /usr/local/mdsplus/bin*/dw*
%exclude /usr/local/mdsplus/bin*/act*
%exclude /usr/local/mdsplus/bin*/traverser
%exclude /usr/local/mdsplus/lib*/libXmds*

%exclude /usr/local/mdsplus/bin*/j*
%exclude /usr/local/mdsplus/lib*/libJava*

%exclude /usr/local/mdsplus/bin*/mdsccl
%exclude /usr/local/mdsplus/bin*/mdscts
%exclude /usr/local/mdsplus/lib*/libCam*
%exclude /usr/local/mdsplus/lib*/libcts*
%exclude /usr/local/mdsplus/lib*/libccl*
%exclude /usr/local/mdsplus/lib*/libRemCam*

%exclude /usr/local/mdsplus/lib*/*.a

%exclude /usr/local/mdsplus/lib*/libI*

%exclude /usr/local/mdsplus/lib*/libMdsSql*

%exclude /usr/local/mdsplus/lib*/libMit*
%exclude /usr/local/mdsplus/lib*/libMIT*
%exclude /usr/local/mdsplus/tdi/MitDevices

%exclude /usr/local/mdsplus/tdi/RfxDevices

%files java
/usr/local/mdsplus/java
/usr/local/mdsplus/bin*/j*
/usr/local/mdsplus/lib*/libJava*

%files idl
/usr/local/mdsplus/idl

%files labview
/usr/local/mdsplus/LabView

%files matlab
/usr/local/mdsplus/matlab

%files motif
/usr/local/mdsplus/bin*/dw*
/usr/local/mdsplus/bin*/act*
/usr/local/mdsplus/bin*/traverser
/usr/local/mdsplus/lib*/libXmds*
/usr/local/mdsplus/uid*

%files camac
/usr/local/mdsplus/bin*/mdsccl
/usr/local/mdsplus/bin*/mdscts
/usr/local/mdsplus/lib*/libCam*
/usr/local/mdsplus/lib*/libcts*
/usr/local/mdsplus/lib*/libccl*
/usr/local/mdsplus/lib*/libRemCam*

%files mitdevices
/usr/local/mdsplus/lib*/libMit*
/usr/local/mdsplus/lib*/libMIT*
/usr/local/mdsplus/tdi/MitDevices

%files rfxdevices
/usr/local/mdsplus/tdi/RfxDevices

%files python
/usr/local/mdsplus/mdsobjects/python

%files devel
/usr/local/mdsplus/include
/usr/local/mdsplus/lib*/*.a

%files mssql
/usr/local/mdsplus/lib*/libMdsSql*

%changelog
* Wed Aug 29 2000 Basil P. DUVAL <basil.duval@epfl.ch>
- version 1.0
