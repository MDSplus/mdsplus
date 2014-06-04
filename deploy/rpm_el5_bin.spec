%define debug_package %{nil}
%global _missing_build_ids_terminate_build 0
Name: mdsplus%{?rflavor}
Version: %{mdsplus_version}
Release: %{mdsplus_release}%{dist}
License: BSD Open Source - Copyright (c) 2010, Massachusetts Institute of Technology All rights reserved.
Source: http://www.mdsplus.org/dist/SOURCES/mdsplus%{?rflavor}-%{mdsplus_version}-%{mdsplus_release}.tgz
URL: http://www.mdsplus.org
Vendor: Massachusetts Institute of Technology
Packager: Plasma Science and Fusion Center <mdsplus@www.mdsplus.org>
Summary: The MDSplus Data System
Group: Applications/Archiving
Prefix: /usr/local
Summary: MDSplus Data Acquisition System
AutoReqProv: yes
requires: mdsplus%{?rflavor}-java = %{version}-%{release}
#requires: mdsplus%{?rflavor}-mitdevices = %{version}-%{release}
#requires: mdsplus%{?rflavor}-epics = %{version}-%{release}
#requires: mdsplus%{?rflavor}-idl = %{version}-%{release}
#requires: mdsplus%{?rflavor}-gsi = %{version}-%{release}
#requires: mdsplus%{?rflavor}-rfxdevices = %{version}-%{release}
#requires: mdsplus%{?rflavor}-php = %{version}-%{release}
#requires: mdsplus%{?rflavor}-kbsidevices = %{version}-%{release}
#requires: mdsplus%{?rflavor}-labview = %{version}-%{release}
#requires: mdsplus%{?rflavor}-matlab = %{version}-%{release}
requires: mdsplus%{?rflavor}-motif = %{version}-%{release}
requires: mdsplus%{?rflavor}-python = %{version}-%{release}
#requires: mdsplus%{?rflavor}-hdf5 = %{version}-%{release}
#requires: mdsplus%{?rflavor}-devel = %{version}-%{release}
#requires: mdsplus%{?rflavor}-camac = %{version}-%{release}
requires: mdsplus%{?rflavor}-kernel = %{version}-%{release}
#requires: mdsplus%{?rflavor}-d3d = %{version}-%{release}
#requires: mdsplus%{?rflavor}-mssql = %{version}-%{release}

%description
The top level MDSplus installation package. Install this package only if you want all
of the MDSplus packages installed on your system. Otherwise select only the packages
that you need such as mdsplus-python or mdsplus-java which will pull in any other
required packages automatically.

%prep
%setup -q -n mdsplus%{?rflavor}-%{mdsplus_version}-%{mdsplus_release}

%build

./configure --prefix=$RPM_BUILD_ROOT/usr/local/mdsplus \
            --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus \
            --enable-nodebug \
	    --enable-mdsip_connections \
            --with-labview=$LABVIEW_DIR \
            --with-jdk=$JDK_DIR \
	    --with-idl=$IDL_DIR \
	    --with-gsi=/usr:gcc%{BITS} \
            --host=%{_arch}-linux
%__make clean
env LANG=en_US.UTF-8 %__make

%install

pyflavor=%{?rflavor}
pyflavor=${pyflavor:1}-
env MDSPLUS_VERSION="${pyflavor}%{version}.%{release}" %__make install
%__mv $RPM_BUILD_ROOT/usr/local/mdsplus/lib $RPM_BUILD_ROOT/usr/local/mdsplus/lib%{BITS}
%__mv $RPM_BUILD_ROOT/usr/local/mdsplus/bin $RPM_BUILD_ROOT/usr/local/mdsplus/bin%{BITS}
mkdir -p $RPM_BUILD_ROOT/etc/yum.repos.d
mkdir -p $RPM_BUILD_ROOT/etc/pki/rpm-gpg/
cp ${WORKSPACE}/RPM-GPG-KEY-MDSplus $RPM_BUILD_ROOT/etc/pki/rpm-gpg/
outfile=$RPM_BUILD_ROOT/etc/yum.repos.d/mdsplus-%flavor.repo
if [ "%flavor" == "stable" ]
then
  echo [MDSplus] > $outfile
else
  echo [MDSplus-%flavor] > $outfile
fi
cat - >> $outfile <<EOF
name=MDSplus-%flavor 
baseurl=http://www.mdsplus.org/dist/%{DIST}/%{flavor}/RPMS
enabled=1
EOF
cat - >>$outfile <<EOF 
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus
metadata_expire=300
EOF


%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

######################### JAVA APPS ###############################
%package java_bin
Summary: Java Applications
Group: Applications/Archiving
%description java_bin
Java applications and classes
%files java_bin
%defattr(-,root,root)
/usr/local/mdsplus/bin??/j*
/usr/local/mdsplus/lib??/libJava*
%post java_bin
ldconfig >/dev/null 2>&1
%postun java_bin
if [ "$1" == "0" ]
then
 %__rm -f /usr/share/applications/mdsplus/java 2>/dev/null
 ldconfig >/dev/null 2>&1
fi

################ MIT Devices ###################################
%package mitdevices_bin
Summary: Support for MIT data acquisition devices
Group: Applications/Archiving
%description mitdevices_bin
Support for MIT Data acquisition devices
%files mitdevices_bin
%defattr(-,root,root)
/usr/local/mdsplus/lib??/libMit*
/usr/local/mdsplus/lib??/libMIT*
/usr/local/mdsplus/tdi/MitDevices
%exclude /usr/local/mdsplus/lib??/*.a
%post mitdevices_bin
ldconfig >/dev/null 2>&1
%postun mitdevices_bin
ldconfig >/dev/null 2>&1

############## IDL #############################################
%package idl_bin
Summary: ITT IDL extensions
Group: Applications/Archiving
Requires: mdsplus%{?rflavor}-kernel = %{version}-%{release}
%description idl_bin
IDL (ITT Interactive Data Language) extensions for MDSplus
%files idl_bin
%defattr(-,root,root)
/usr/local/mdsplus/lib??/lib*Idl*
%exclude /usr/local/mdsplus/lib??/*.a
%post idl_bin
ldconfig >/dev/null 2>&1
%postun idl_bin
ldconfig >/dev/null 2>&1

############## Globus Security ################################
%package gsi_bin
Summary: Support for secure MDSplus and Fusiongrid
Group: Applications/Archiving
%description gsi_bin
Support for secure MDSplus and Fusiongrid
%files gsi_bin
%defattr(-,root,root)
/usr/local/mdsplus/lib??/libRoam*
/usr/local/mdsplus/lib??/libMdsIpGSI*
/usr/local/mdsplus/bin??/roam_check_access
/usr/local/mdsplus/bin??/mdsipsd
%exclude /usr/local/mdsplus/lib??/*.a
%post gsi_bin
ldconfig >/dev/null 2>&1
%postun gsi_bin
ldconfig >/dev/null 2>&1
%triggerin gsi_bin -- xinetd
if [ ! -r /etc/xinetd.d/mdsips ]
then
  cp $RPM_INSTALL_PREFIX/mdsplus/rpm/mdsipsd.xinetd /etc/xinetd.d/mdsips
  if ( ! grep '^mdsips[[:space:]]' /etc/services >/dev/null 2>&1)
  then
    echo 'mdsips 8200/tcp # MDSplus mdsip service' >> /etc/services
  fi
fi
/sbin/service xinetd reload

####################### LabView ##############################
%package labview_bin
Summary: National Instruments Labview extensions
Group: Applications/Archiving
%description labview_bin
National Instruments Labview interface to MDSplus
%files labview_bin
%defattr(-,root,root)
/usr/local/mdsplus/lib??/*LV*
%exclude /usr/local/mdsplus/lib??/*.a

##################### Motif APPS #############################
%package motif_bin
Summary: X-Windows Motif based application
Group: Applications/Archiving
%description motif_bin
X-Windows applications which use the Motif window system. This package provides the following applications:
dwscope, dwpad, traverser, actions, actmon
%files motif_bin
%defattr(-,root,root)
/usr/local/mdsplus/bin??/dw*
/usr/local/mdsplus/bin??/act*
/usr/local/mdsplus/bin??/traverser
/usr/local/mdsplus/lib??/libXmds*
/usr/local/mdsplus/uid*
%exclude /usr/local/mdsplus/lib??/*.a
%post motif_bin
if [ ! -d $RPM_INSTALL_PREFIX/mdsplus/uid ]
then
  if [ -d $RPM_INSTALL_PREFIX/mdsplus/uid64 ]
  then
    ln -sf uid64 $RPM_INSTALL_PREFIX/mdsplus/uid
  else
    ln -sf uid32 $RPM_INSTALL_PREFIX/mdsplus/uid
  fi
fi
ldconfig >/dev/null 2>&1
%preun motif_bin
if [ -h $RPM_INSTALL_PREFIX/mdsplus/uid ]
then
  %__rm -f $RPM_INSTALL_PREFIX/mdsplus/uid
fi
%postun motif_bin
if [ "$1" == "0" ]
then
  ldconfig >/dev/null 2>&1
fi

################## HDF5 APPS
%package hdf5_bin
Summary: MDSplus/HDF5 integration
Group: Applications/Archiving
%description hdf5_bin
MDSplus/HDF5 integration
%files hdf5_bin
%defattr(-,root,root)
/usr/local/mdsplus/lib??/libhdf5*
/usr/local/mdsplus/tdi/hdf5
/usr/local/mdsplus/bin??/hdf5*
/usr/local/mdsplus/bin??/*HDF5
%exclude /usr/local/mdsplus/lib??/*.a
%post hdf5_bin
ldconfig >/dev/null 2>&1
%postun hdf5_bin
ldconfig >/dev/null 2>&1


################# Development Files ##########################
%package devel_bin
Summary: Header files and static libraries for code development
Group: Applications/Archiving
%description devel_bin
Header files and static libraries for code development
%files devel_bin
%defattr(-,root,root)
/usr/local/mdsplus/lib??/*.a


################# CAMAC Support ##############################
%package camac_bin
Summary: Support for CAMAC devices
Group: Applications/Archiving
%description camac_bin
Support for accessing CAMAC devices
%files camac_bin
%defattr(-,root,root)
/usr/local/mdsplus/bin??/mdsccl
/usr/local/mdsplus/bin??/mdscts
/usr/local/mdsplus/lib??/libCam*
/usr/local/mdsplus/lib??/libcts*
/usr/local/mdsplus/lib??/libccl*
/usr/local/mdsplus/lib??/libRemCam*
%exclude /usr/local/mdsplus/lib??/*.a
%post camac_bin
ldconfig >/dev/null 2>&1
%postun camac_bin
ldconfig >/dev/null 2>&1


####################### MDSplus Kernel ###########################
%package kernel_bin
Summary: MDSplus core system
Group: Applications/Archiving
%description kernel_bin
Core applications, libraries and configuration files
%files kernel_bin
%defattr(-,root,root)
/usr/local/mdsplus/bin??
/usr/local/mdsplus/lib??
%exclude /usr/local/mdsplus/bin??/dw*
%exclude /usr/local/mdsplus/bin??/act*
%exclude /usr/local/mdsplus/bin??/traverser
%exclude /usr/local/mdsplus/lib??/libXmds*
%exclude /usr/local/mdsplus/bin??/j*
%exclude /usr/local/mdsplus/lib??/libJava*
%exclude /usr/local/mdsplus/lib??/*LV*
%exclude /usr/local/mdsplus/bin??/mdsccl
%exclude /usr/local/mdsplus/bin??/mdscts
%exclude /usr/local/mdsplus/lib??/libCam*
%exclude /usr/local/mdsplus/lib??/libcts*
%exclude /usr/local/mdsplus/lib??/libccl*
%exclude /usr/local/mdsplus/lib??/libRemCam*
%exclude /usr/local/mdsplus/lib??/*.a
%exclude /usr/local/mdsplus/lib??/lib*Idl*
%exclude /usr/local/mdsplus/lib??/lib*Sql*
%exclude /usr/local/mdsplus/lib??/libMit*
%exclude /usr/local/mdsplus/lib??/libMIT*
%exclude /usr/local/mdsplus/bin??/reboot_dtaq_satelite
%exclude /usr/local/mdsplus/lib??/libRoam*
%exclude /usr/local/mdsplus/lib??/libMdsIpGSI*
%exclude /usr/local/mdsplus/bin??/roam_check_access
%exclude /usr/local/mdsplus/bin??/mdsipsd
%exclude /usr/local/mdsplus/lib??/libhdf5*
%exclude /usr/local/mdsplus/bin??/hdf5*
%exclude /usr/local/mdsplus/bin??/*HDF5
%post kernel_bin
if [ -d /etc/ld.so.conf.d ]
then
  %__rm -f /etc/ld.so.conf.d/mdsplus.conf 2>/dev/null
  touch /etc/ld.so.conf.d/mdsplus.conf
  for l in lib lib32 lib64
  do
    if [ ! -h $RPM_INSTALL_PREFIX/mdsplus/$l -a -d $RPM_INSTALL_PREFIX/mdsplus/$l ]
    then
      echo "$RPM_INSTALL_PREFIX/mdsplus/$l" >> /etc/ld.so.conf.d/mdsplus.conf
    fi
  done
  ldconfig > /dev/null 2>&1
fi
if [ -d $RPM_INSTALL_PREFIX/mdsplus/bin64 ]
then
  bits=64
else
  bits=32
fi
if [ -d $RPM_INSTALL_PREFIX/mdsplus/bin${bits} ]
then
  if [ ! -h $RPM_INSTALL_PREFIX/mdsplus/bin ]
  then
    ln -sf bin${bits} $RPM_INSTALL_PREFIX/mdsplus/bin
  fi
fi
if [ -d $RPM_INSTALL_PREFIX/mdsplus/lib${bits} ]
then
  if [ ! -h $RPM_INSTALL_PREFIX/mdsplus/lib ]
  then
    ln -sf lib${bits} $RPM_INSTALL_PREFIX/mdsplus/lib
  fi
fi
%preun kernel_bin
if [ "$1" == "0" ]
then
  %__rm -f $RPM_INSTALL_PREFIX/mdsplus/bin
  %__rm -f $RPM_INSTALL_PREFIX/mdsplus/lib
fi
%postun kernel_bin
if [ "$1" == "0" ]
then
  %__rm -f /etc/ld.so.conf.d/mdsplus.conf 2>/dev/null 
  ldconfig
fi

################### Microsoft SQL Server connection ############
%package mssql
Summary: Interface to mssql databases
Group: Applications/Archiving
%description mssql
Interface to mssql databases
%files mssql
%defattr(-,root,root)
/usr/local/mdsplus/lib??/lib*Sql*
%exclude /usr/local/mdsplus/lib??/*.a
%post mssql
ldconfig >/dev/null 2>&1
%postun mssql
ldconfig >/dev/null 2>&1


####################### Yum Repository Setup ############################
%package repo
Summary: Yum Repository Setup
Group: Applications/Archiving
Prefix: /
Summary: MDSplus Data Acquisition System
AutoReqProv: no
%description repo
MDSplus Yum repository setup
%files repo
%defattr(-,root,root)
/etc/yum.repos.d/mdsplus-%flavor.repo
/etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus
%post repo
dummy=$(rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus 2>/dev/null)
%postun repo
nohup rpm -e gpg-pubkey-b09cb563 >dev/null 2>&1 &

