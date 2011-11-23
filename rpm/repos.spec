Name: mdsplus-%flavor-repo
Version: 2.0
Release: 0%{dist}
License: BSD Open Source
Summary: The MDSplus Data System
Group: Applications/Acquisition
Prefix: /
Summary: MDSplus Data Acquisition System
AutoReqProv: no

%description
MDSplus Yum repository setup

%prep
%build

%install
mkdir -p $RPM_BUILD_ROOT/etc/yum.repos.d
mkdir -p $RPM_BUILD_ROOT/etc/pki/rpm-gpg/
cp /home/mdsplus/RPM-GPG-KEY-MDSplus $RPM_BUILD_ROOT/etc/pki/rpm-gpg/
outfile=$RPM_BUILD_ROOT/etc/yum.repos.d/mdsplus-%flavor.repo
cat - > $outfile <<EOF
[MDSplus]
name=MDSplus-%flavor 
baseurl=http://www.mdsplus.org/repo/rhel%{s_dist}-%{flavor}/RPMS
enabled=1
EOF
if [ "%_target" != "i686-linux" ]
then
echo "exclude=*i686*" >> $outfile
fi
cat - >>$outfile <<EOF 
gpgcheck=1
metadata_expire=300
EOF


%clean
rm -Rf $RPM_BUILD_ROOT

%files
/etc/yum.repos.d/mdsplus-%flavor.repo
/etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus

%post
rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus

