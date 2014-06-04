%define debug_package %{nil}
%global _missing_build_ids_terminate_build 0
%global _unpackaged_files_terminate_build 0
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
%package java
Summary: Java Applications
Group: Applications/Archiving
BuildArch: noarch
requires: mdsplus%{?rflavor}-java_bin = %{version}-%{release}
%description java
Java applications and classes
%files java
%defattr(-,root,root)
/usr/local/mdsplus/java
/usr/local/mdsplus/desktop/java
/usr/local/mdsplus/tdi/java
%exclude /usr/local/mdsplus/java/classes/jDevices.jar
/usr/local/mdsplus/pixmaps/dwscope.png
/usr/local/mdsplus/pixmaps/traverser.png
%post java
if [ -d /usr/share/applications/mdsplus ]
then
  ln -sf $RPM_INSTALL_PREFIX/mdsplus/desktop/java /usr/share/applications/mdsplus/
fi
%postun java
if [ "$1" == "0" ]
then
 %__rm -f /usr/share/applications/mdsplus/java 2>/dev/null
fi


################ MIT Devices ###################################
%package mitdevices
Summary: Support for MIT data acquisition devices
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-python = %{version}-%{release}
Requires: mdsplus%{?rflavor}-mitdevices_bin = %{version}-%{release}
%description mitdevices
Support for MIT Data acquisition devices
%files mitdevices
%defattr(-,root,root)
/usr/local/mdsplus/tdi/MitDevices
%post mitdevices
pushd $RPM_INSTALL_PREFIX/mdsplus/tdi/MitDevices
%__rm -Rf %{python_sitelib}/MitDevices*
%__rm -Rf build dist
%__python setup.py -q install
%__rm -Rf build dist MitDevices.egg-info
popd
%preun mitdevices
if [ "$1" == "0" ]
then
  %__rm -Rf %{python_sitearch}/MitDevices*
fi

############## IDL #############################################
%package idl
Summary: ITT IDL extensions
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-idl_bin = %{version}-%{release} 
%description idl
IDL (ITT Interactive Data Language) extensions for MDSplus
%files idl
%defattr(-,root,root)
/usr/local/mdsplus/idl
%exclude /usr/local/mdsplus/idl/camac

############## Globus Security ################################
%package gsi
Summary: Support for secure MDSplus and Fusiongrid
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-gsi_bin = %{version}-%{release}
Requires: mdsplus%{?rflavor}-kernel = %{version}-%{release}
%description gsi
Support for secure MDSplus and Fusiongrid
%files gsi
%defattr(-,root,root)
/usr/local/mdsplus/tdi/roam

####################### LabView ##############################
%package labview
Summary: National Instruments Labview extensions
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-labview_bin = %{version}-%{release}
Requires: mdsplus%{?rflavor}-kernel = %{version}-%{release}
%description labview
National Instruments Labview interface to MDSplus
%files labview
%defattr(-,root,root)
/usr/local/mdsplus/LabView

##################### Motif APPS #############################
%package motif
Summary: X-Windows Motif based application
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-motif_bin = %{version}-%{release}
Requires: mdsplus%{?rflavor}-kernel = %{version}-%{release}
%description motif
X-Windows applications which use the Motif window system. This package provides the following applications:
dwscope, dwpad, traverser, actions, actmon
%files motif
%defattr(-,root,root)
/usr/local/mdsplus/desktop/motif
/usr/local/mdsplus/pixmaps/dwpad.png
/usr/local/mdsplus/pixmaps/dwscope.png
/usr/local/mdsplus/pixmaps/traverser.png
%post motif
if [ -d /usr/share/applications/mdsplus ]
then
  ln -sf $RPM_INSTALL_PREFIX/mdsplus/desktop/motif /usr/share/applications/mdsplus/
fi
%postun motif
if [ "$1" == "0" ]
then
  %__rm -f /usr/share/applications/mdsplus/motif 2>/dev/null
fi


################## HDF5 APPS
%package hdf5
Summary: MDSplus/HDF5 integration
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-hdf5_bin = %{version}-%{release}
Requires: mdsplus%{?rflavor}-kernel = %{version}-%{release}
%description hdf5
MDSplus/HDF5 integration
%files hdf5
%defattr(-,root,root)
/usr/local/mdsplus/tdi/hdf5

################# Development Files ##########################
%package devel
Summary: Header files and static libraries for code development
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-devel_bin = %{version}-%{release}
%description devel
Header files and static libraries for code development
%files devel
%defattr(-,root,root)
/usr/local/mdsplus/include

################# CAMAC Support ##############################
%package camac
Summary: Support for CAMAC devices
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-camac_bin = %{version}-%{release}
Requires: mdsplus%{?rflavor}-kernel = %{version}-%{release}
%description camac
Support for accessing CAMAC devices
%files camac
%defattr(-,root,root)
/usr/local/mdsplus/idl/camac
/usr/local/mdsplus/tdi/camac

####################### MDSplus Kernel ###########################
%package kernel
Summary: MDSplus core system
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-kernel_bin = %{version}-%{release}
%description kernel
Core applications, libraries and configuration files
%files kernel
%defattr(-,root,root)
%dir /usr/local/mdsplus/idl
%dir /usr/local/mdsplus/mdsobjects
/usr/local/mdsplus/MDSplus-License.txt
/usr/local/mdsplus/ChangeLog
/usr/local/mdsplus/etc
/usr/local/mdsplus/share
/usr/local/mdsplus/rpm
/usr/local/mdsplus/setup.csh
/usr/local/mdsplus/setup.sh
/usr/local/mdsplus/tdi
/usr/local/mdsplus/trees
/usr/local/mdsplus/desktop/mdsplus.directory
/usr/local/mdsplus/desktop/mdsplus.menu
/usr/local/mdsplus/desktop/kernel
/usr/local/mdsplus/pixmaps/small_mdsplus_logo.jpg
/usr/local/mdsplus/pixmaps/Tcl.png
/usr/local/mdsplus/pixmaps/Tdi.png
%exclude /usr/local/mdsplus/tdi/MitDevices
%exclude /usr/local/mdsplus/tdi/RfxDevices
%exclude /usr/local/mdsplus/tdi/KbsiDevices
%exclude /usr/local/mdsplus/tdi/camac
%exclude /usr/local/mdsplus/tdi/java
%exclude /usr/local/mdsplus/tdi/d3d
%exclude /usr/local/mdsplus/tdi/roam
%exclude /usr/local/mdsplus/tdi/hdf5
%post kernel
if [ -d /etc/profile.d ]
then
  ln -sf $RPM_INSTALL_PREFIX/mdsplus/setup.sh /etc/profile.d/mdsplus.sh
  ln -sf $RPM_INSTALL_PREFIX/mdsplus/setup.csh /etc/profile.d/mdsplus.csh
fi
if [ -d /etc/xdg/menus/applications-merged ]
then
  ln -sf $RPM_INSTALL_PREFIX/mdsplus/desktop/mdsplus.menu /etc/xdg/menus/applications-merged/
fi
if [ -d /usr/share/desktop-directories ]
then
  ln -sf $RPM_INSTALL_PREFIX/mdsplus/desktop/mdsplus.directory /usr/share/desktop-directories/
fi
if [ -d /usr/share/applications ]
then
  mkdir /usr/share/applications/mdsplus
  ln -sf $RPM_INSTALL_PREFIX/mdsplus/desktop/kernel /usr/share/applications/mdsplus/
fi
%preun kernel
if [ "$1" == "0" ]
then
  %__rm -f /etc/profile.d/mdsplus.sh 2>/dev/null
  %__rm -f /etc/profile.d/mdsplus.csh 2>/dev/null
  %__rm -f /etc/.mdsplus_dir 2>/dev/null
  %__rm -f /etc/xdg/menus/applications-merged/mdsplus.menu
  %__rm -f /usr/share/desktop-directories/mdsplus.directory
  %__rm -Rf /usr/share/applications/mdsplus
  if [ -r /etc/xinetd.d/mdsip ]
  then
    %__rm -f /etc/xinetd.d/mdsip
    /sbin/service xinetd reload
  fi
  if ( grep '^mdsip[[:space::]]' /etc/services >/dev/null 2>&1 )
  then
    tmpfile=$(mktemp)
    if ( grep -v '^mdsip[[:space::]]' /etc/services > $tmpfile )
    then
      %__mv /etc/services /etc/services.rpmsave
      %__mv $tmpfile /etc/services
    fi
  fi 
fi
%postun kernel
if [ "$1" == "0" ]
then
  %__rm -Rf $RPM_INSTALL_PREFIX/mdsplus/{desktop,pixmaps}
fi

%triggerin kernel -- xinetd
if [ ! -r /etx/xinetd.d/mdsip ]
then
  cp $RPM_INSTALL_PREFIX/mdsplus/rpm/mdsipd.xinetd /etc/xinetd.d/mdsip
  if ( ! grep '^mdsip[[:space:]]' /etc/services >/dev/null 2>&1)
  then
    echo 'mdsip 8000/tcp # MDSplus mdsip service' >> /etc/services
  fi
fi
/sbin/service xinetd reload

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


################### EPICS Interface Configuration files ############
%package epics
Summary: MDSplus/EPICS integration
Group: Applications/Archiving
BuildArch: noarch
%description epics
MDSplus/EPICS integration
%files epics
%defattr(-,root,root)
/usr/local/mdsplus/epics

################### RFX Device Support #############################
%package rfxdevices
Summary: Support for RFX data acquisition devices
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-java = %{version}-%{release}
Requires: mdsplus%{?rflavor}-python = %{version}-%{release}
%description rfxdevices
Support for RFX Data acquisition devices
%files rfxdevices
%defattr(-,root,root)
/usr/local/mdsplus/tdi/RfxDevices
/usr/local/mdsplus/java/classes/jDevices.jar

################## PHP CGI Support ##############################
%package php
Summary: php interface to MDSplus
Group: Applications/Archiving
BuildArch: noarch
%description php
php interface to MDSplus
%files php
%defattr(-,root,root)
/usr/local/mdsplus/php

################### KBSI Device Support ###########################
%package kbsidevices
Summary: Support for KBSI data acquisition devices
Group: Applications/Archiving
BuildArch: noarch
%description kbsidevices
Support for KBSI Data acquisition devices
%files kbsidevices
%defattr(-,root,root)
/usr/local/mdsplus/tdi/KbsiDevices

#################### MATLAB Interface #############################
%package matlab
Summary: Mathworks MATLAB extensions
Group: Applications/Archiving
BuildArch: noarch
Requires: mdsplus%{?rflavor}-java = %{version}-%{release}
%description matlab
Mathworks MATLAB extensions
%files matlab
%defattr(-,root,root)
/usr/local/mdsplus/matlab

################### Python Interface #############################
%package python
Summary: Python interface to MDSplus
Group: Applications/Archiving
requires: mdsplus%{?rflavor}-kernel,python,numpy,python-ctypes
BuildArch: noarch
%description python
Python interface to MDSplus
%files python
%defattr(-,root,root)
/usr/local/mdsplus/mdsobjects/python
%exclude
%post python
%__rm -Rf %{python_sitelib}/MDSplus-*
pushd $RPM_INSTALL_PREFIX/mdsplus/mdsobjects/python >/dev/null 2>&1
%__python setup.py -q install
%__rm -Rf build dist MDSplus.egg-info
popd >/dev/null 2>&1
%preun python
if [ "$1" == "0" ]
then
  %__rm -Rf %{python_sitelib}/MDSplus-*
fi

######################## D3D Data Interface ########################
%package d3d
Summary: TDI functions used at D3D experiment at General Atomics
Group: Applications/Archiving
BuildArch: noarch
%description d3d
TDI functions used at the D3D experiment at General Atomics.
%files d3d
%defattr(-,root,root)
/usr/local/mdsplus/tdi/d3d

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

